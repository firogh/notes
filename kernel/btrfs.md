---
tags: kernel
title: btrfs
date: 2015-02-27 15:46:12
category: kernel
---
int __btrfs_cow_block(struct btrfs_trans_handle *trans, struct btrfs_root *root,
			     struct extent_buffer *buf,
			     struct extent_buffer *parent, int parent_slot,
			     struct extent_buffer **cow_ret,
			     u64 search_start, u64 empty_size)
{
	struct extent_buffer *cow;
	struct btrfs_disk_key disk_key;
	int level;

	WARN_ON(root->ref_cows && trans->transid !=
		root->fs_info->running_transaction->transid);
	WARN_ON(root->ref_cows && trans->transid != root->last_trans);

	level = btrfs_header_level(buf);
	//就像我说的这里level是0.

	//为什么要获得这个叶子结点的第一个item的key呢？第一个是由0来索引的，即偏移量为0.
	if (level == 0)
		btrfs_item_key(buf, &disk_key, 0);
	else
		btrfs_node_key(buf, &disk_key, 0);

	//从函数的名字上分析我们可以知道，它是用来获得一个空闲的block区在硬盘上。
	//而这里的search_start is a hint。他是由我们即将提交的这个leaf在硬盘的开始位置，按G对齐后的值。
	//empty_size 亦为0. 
	cow = btrfs_alloc_free_block(trans, root, buf->len,
				     root->root_key.objectid, &disk_key,
				     level, search_start, empty_size);
	if (IS_ERR(cow))
		return PTR_ERR(cow);

	copy_extent_buffer(cow, buf, 0, 0, cow->len);
	btrfs_set_header_bytenr(cow, cow->start);
	btrfs_set_header_generation(cow, trans->transid);
	btrfs_set_header_backref_rev(cow, BTRFS_MIXED_BACKREF_REV);
	btrfs_clear_header_flag(cow, BTRFS_HEADER_FLAG_WRITTEN |
				     BTRFS_HEADER_FLAG_RELOC);
	if (root->root_key.objectid == BTRFS_TREE_RELOC_OBJECTID)
		btrfs_set_header_flag(cow, BTRFS_HEADER_FLAG_RELOC);
	else
		btrfs_set_header_owner(cow, root->root_key.objectid);

	write_extent_buffer(cow, root->fs_info->fsid,
			    (unsigned long)btrfs_header_fsid(cow),
			    BTRFS_FSID_SIZE);

	WARN_ON(btrfs_header_generation(buf) > trans->transid);

	update_ref_for_cow(trans, root, buf, cow);

	if (buf == root->node) {
		root->node = cow;
		extent_buffer_get(cow);

		btrfs_free_extent(trans, root, buf->start, buf->len,
				  0, root->root_key.objectid, level, 0);
		free_extent_buffer(buf);
		add_root_to_dirty_list(root);
	} else {
		btrfs_set_node_blockptr(parent, parent_slot,
					cow->start);
		WARN_ON(trans->transid == 0);
		btrfs_set_node_ptr_generation(parent, parent_slot,
					      trans->transid);
		btrfs_mark_buffer_dirty(parent);
		WARN_ON(btrfs_header_generation(parent) != trans->transid);

		btrfs_free_extent(trans, root, buf->start, buf->len,
				  0, root->root_key.objectid, level, 1);
	}
	free_extent_buffer(buf);
	btrfs_mark_buffer_dirty(cow);
	*cow_ret = cow;
	return 0;
}


struct extent_buffer *btrfs_alloc_free_block(struct btrfs_trans_handle *trans,
					struct btrfs_root *root,
					u32 blocksize, u64 root_objectid,
					struct btrfs_disk_key *key, int level,
					u64 hint, u64 empty_size)
{
	struct btrfs_key ins;
	int ret;
	struct extent_buffer *buf;

	//我们也能看到这里的参数&ins，是用来做返回值的，且是btrfs_key.
	//至于返回的是什么，我们跳进去看一看。我猜这就是这个新的block插入位置的key值。
	ret = alloc_tree_block(trans, root, blocksize, root_objectid,trans->transid, 0, key, level,empty_size, hint, (u64)-1, &ins);
	if (ret) {
		BUG_ON(ret > 0);
		return ERR_PTR(ret);
	}

	buf = btrfs_find_create_tree_block(root, ins.objectid, blocksize);
	if (!buf) {
		btrfs_free_extent(trans, root, ins.objectid, ins.offset, 0, root->root_key.objectid, level, 0);
		BUG_ON(1);
		return ERR_PTR(-ENOMEM);
	}
	btrfs_set_buffer_uptodate(buf);
	trans->blocks_used++;

	return buf;
}

static int alloc_tree_block(struct btrfs_trans_handle *trans,struct btrfs_root *root, u64 num_bytes, u64 root_objectid, u64 generation,
		u64 flags, struct btrfs_disk_key *key,int level, u64 empty_size, u64 hint_byte,u64 search_end, struct btrfs_key *ins)
{
	//蛋疼的参数呀，12个之多~~
	int ret;
	//真正蛋疼的地方。这个reserve 什么意思呢？是保留，预留的意思，为什么而留呢？
	//推测1disk预留一定的空间，这个比较不靠谱。推测2为这个block所留的，也就是，多分配出来的为这个block。
	//推测3这个reserve就是表示我们要预先保留numbytes 大小的extent。
	//0表示不是data block group。大致意思就是标识这个extent所属的区域不是data block group。I am not sure.
	//这些组是存在于RAM中。
	ret = btrfs_reserve_extent(trans, root, num_bytes, empty_size,hint_byte, search_end, ins, 0);
	BUG_ON(ret);

	if (root_objectid == BTRFS_EXTENT_TREE_OBJECTID) {
		struct pending_extent_op *extent_op;

		extent_op = kmalloc(sizeof(*extent_op), GFP_NOFS);
		BUG_ON(!extent_op);

		extent_op->type = PENDING_EXTENT_INSERT;
		extent_op->bytenr = ins->objectid;
		extent_op->num_bytes = ins->offset;
		extent_op->level = level;
		extent_op->flags = flags;
		memcpy(&extent_op->key, key, sizeof(*key));

		set_extent_bits(&root->fs_info->extent_ins, ins->objectid,
				ins->objectid + ins->offset - 1,
				EXTENT_LOCKED, GFP_NOFS);
		set_state_private(&root->fs_info->extent_ins,
				  ins->objectid, (unsigned long)extent_op);
	} else {
		ret = alloc_reserved_tree_block(trans, root, root_objectid,
						generation, flags,
						key, level, ins);
		finish_current_insert(trans, root->fs_info->extent_root);
		del_pending_extents(trans, root->fs_info->extent_root);
	}
	return ret;
}


static int btrfs_reserve_extent(struct btrfs_trans_handle *trans,struct btrfs_root *root,u64 num_bytes, u64 empty_size,
				u64 hint_byte, u64 search_end,struct btrfs_key *ins, int data)
{
	int ret;
	u64 search_start = 0;
	u64 alloc_profile;
	struct btrfs_fs_info *info = root->fs_info;

	//这个只有在btrfs convert时候才会被初始化，这里我们掠过~~
	if (info->extent_ops) {
		struct btrfs_extent_ops *ops = info->extent_ops;
		ret = ops->alloc_extent(root, num_bytes, hint_byte, ins);
		BUG_ON(ret);
		goto found;
	}

	//data_alloc_profile = (u64)-1; 这个就像mask
	//avail_data_alloc_bits 就是raid1 ，raid10 ，raid0 ,dup这些的bit位，是由我们传入的参数确定。也是u64类型。这个才是威力所在。
	if (data) { //上面传下来这里是0.
		alloc_profile = info->avail_data_alloc_bits &
			        info->data_alloc_profile;
		//所以这个data里面包含的就是两种类别的组第一种是raid1,dup这些用于管理的组类；
		//第二种就是数据实际用途的组类比如，data，metadata，system
		data = BTRFS_BLOCK_GROUP_DATA | alloc_profile;
	} else if ((info->system_allocs > 0 || root == info->chunk_root) &&info->system_allocs >= 0) {
		//system_allocs 这个在make_root_dir 里面设置为1在创建system block group，创建完之后就设置为0
		//那么这个system allocs到底是什么意思。它好象是用来标识是否在创建system block group.
		//所以这个ifcase的解释就是：（当然这是主观的推测）
		//1 如果system block group正在创建 则system_allocs为1,我想创建的步骤中肯定有一步调用这个函数了，呃。。我没找到。
		//2 如果system block group创建完成 system_allocs 为0,则不会满足。也就是说不用创建system block group.
		//3 如果这次创建的是chunk root的system block group就通过。
		//同样avail_system_alloc_bits 也是raid1 raid0 raid10 dup 等
		//system_alloc_profile 依然是u64-1
		alloc_profile = info->avail_system_alloc_bits & info->system_alloc_profile;
		data = BTRFS_BLOCK_GROUP_SYSTEM | alloc_profile;
	} else {//这次调用就是这里了。
		//显然如果前两个 system 和 data 都失败了，这就应该是 metadata。
		alloc_profile = info->avail_metadata_alloc_bits & //同上
			        info->metadata_alloc_profile; //(u64)-1
		data = BTRFS_BLOCK_GROUP_METADATA | alloc_profile;
	}
	//__setup_root()设置为0
	//btrfs_read_fs_root_no_cache设置为1
	//这里的值为0,我是用if和printk判断出来的。
	if (root->ref_cows) {
		if (!(data & BTRFS_BLOCK_GROUP_METADATA)) {
			ret = do_chunk_alloc(trans, root->fs_info->extent_root,
					     num_bytes,
					     BTRFS_BLOCK_GROUP_METADATA);
			BUG_ON(ret);
		}
		ret = do_chunk_alloc(trans, root->fs_info->extent_root,
				     num_bytes + 2 * 1024 * 1024, data);
		BUG_ON(ret);
	}

	WARN_ON(num_bytes < root->sectorsize);
	//很显然这是reserve的核心.
	//这个函数的用途就是找到一个空闲的范围extent.
	ret = find_free_extent(trans, root, num_bytes, empty_size,
			       search_start, search_end, hint_byte, ins,
			       trans->alloc_exclude_start,
			       trans->alloc_exclude_nr, data);
	BUG_ON(ret);
found:
	clear_extent_dirty(&root->fs_info->free_space_cache,
			   ins->objectid, ins->objectid + ins->offset - 1,
			   GFP_NOFS);
	return ret;
}


/*
 * walks the btree of allocated extents and find a hole of a given size.
 * The key ins is changed to record the hole:
 * ins->objectid == block start
 * ins->flags = BTRFS_EXTENT_ITEM_KEY
 * ins->offset == number of blocks
 * Any available blocks before search_start are skipped.
 */
static int noinline find_free_extent(struct btrfs_trans_handle *trans, struct btrfs_root *orig_root, u64 num_bytes, u64 empty_size,
		     u64 search_start, u64 search_end,u64 hint_byte, struct btrfs_key *ins,u64 exclude_start, u64 exclude_nr,int data)
{
	//大致意思基本可以理解，日后必定细看。
	int ret;
	u64 orig_search_start = search_start;
	struct btrfs_root * root = orig_root->fs_info->extent_root;
	struct btrfs_fs_info *info = root->fs_info;
	u64 total_needed = num_bytes;
	struct btrfs_block_group_cache *block_group;
	int full_scan = 0;
	int wrapped = 0;

	WARN_ON(num_bytes < root->sectorsize);
	btrfs_set_key_type(ins, BTRFS_EXTENT_ITEM_KEY);
	//这的含义很显然，为了加快搜索先从hint 开始，失败了再从search start 即0，再找。
	if (hint_byte) {
		block_group = btrfs_lookup_first_block_group(info, hint_byte);
		if (!block_group)
			hint_byte = search_start;// 0
		//data为METADATA，
		block_group = btrfs_find_block_group(root, block_group,hint_byte, data, 1);
	} else {
		//trans->block_group 赋值为NULL。
		block_group = btrfs_find_block_group(root,trans->block_group,search_start, data, 1);
	}

	total_needed += empty_size;

check_failed:
	if (!block_group) {
		block_group = btrfs_lookup_first_block_group(info,
							     search_start);
		if (!block_group)
			block_group = btrfs_lookup_first_block_group(info,
						       orig_search_start);
	}
	ret = find_search_start(root, &block_group, &search_start,
				total_needed, data);
	if (ret)
		goto error;

	search_start = stripe_align(root, search_start);
	ins->objectid = search_start;
	ins->offset = num_bytes;

	if (ins->objectid + num_bytes >
	    block_group->key.objectid + block_group->key.offset) {
		search_start = block_group->key.objectid +
			block_group->key.offset;
		goto new_group;
	}

	if (test_range_bit(&info->extent_ins, ins->objectid,
			   ins->objectid + num_bytes -1, EXTENT_LOCKED, 0)) {
		search_start = ins->objectid + num_bytes;
		goto new_group;
	}

	if (test_range_bit(&info->pinned_extents, ins->objectid,
			   ins->objectid + num_bytes -1, EXTENT_DIRTY, 0)) {
		search_start = ins->objectid + num_bytes;
		goto new_group;
	}

	if (exclude_nr > 0 && (ins->objectid + num_bytes > exclude_start &&
	    ins->objectid < exclude_start + exclude_nr)) {
		search_start = exclude_start + exclude_nr;
		goto new_group;
	}

	if (!(data & BTRFS_BLOCK_GROUP_DATA)) {
		block_group = btrfs_lookup_block_group(info, ins->objectid);
		if (block_group)
			trans->block_group = block_group;
	}
	ins->offset = num_bytes;
	return 0;

new_group:
	block_group = btrfs_lookup_first_block_group(info, search_start);
	if (!block_group) {
		search_start = orig_search_start;
		if (full_scan) {
			ret = -ENOSPC;
			goto error;
		}
		if (wrapped) {
			if (!full_scan)
				total_needed -= empty_size;
			full_scan = 1;
		} else
			wrapped = 1;
	}
	cond_resched();
	block_group = btrfs_find_block_group(root, block_group,
					     search_start, data, 0);
	goto check_failed;

error:
	return ret;
}


struct btrfs_block_group_cache *btrfs_lookup_first_block_group(struct btrfs_fs_info *info,u64 bytenr)
{
	struct extent_io_tree *block_group_cache;
	struct btrfs_block_group_cache *block_group = NULL;
	u64 ptr;
	u64 start;
	u64 end;
	int ret;
	//任何一个group的开始位置应该是在super block的后面。
	bytenr = max_t(u64, bytenr,BTRFS_SUPER_INFO_OFFSET + BTRFS_SUPER_INFO_SIZE);
	//info的block_group_cache 的cache_tree(就是rb root而已)类型的state->rb_root找到 rb_node,在用container of由rb node找到 extent_state
	//而extent_state中private成员，就有我们要找的真.btrfs_block_group_cache!!!
	block_group_cache = &info->block_group_cache;
	//用指针参数返回找到的第一个extent的start 和end。
	ret = find_first_extent_bit(block_group_cache, bytenr, &start, &end,BLOCK_GROUP_DATA | BLOCK_GROUP_METADATA |BLOCK_GROUP_SYSTEM);
	if (ret) {
		return NULL;
	}
	//上面返回的start
	//我觉得这个函数和上面的功能上有重复,在extent_state的private成员保存了btrfs_block_group_cache.h
	ret = get_state_private(block_group_cache, start, &ptr);
	if (ret)
		return NULL;
	block_group = (struct btrfs_block_group_cache *)(unsigned long)ptr;
	return block_group;
}

int find_first_extent_bit(struct extent_io_tree *tree, u64 start,u64 *start_ret, u64 *end_ret, int bits)
{
	struct cache_extent *node;
	struct extent_state *state;
	int ret = 1;

	/*
	 * this search will find all the extents that end after
	 * our range starts.
	 */
	//就像英文注释所说的那样，这个函数就是简单的调用rb search 搜索rb树找到结束于start之后的所有extent
	//当然这些extent都是缓存在我们info block_group_cache的state这棵树上。
	//很有必要来分析一下extent_state.
	//在 fsinfo->block_group_cache也就是extent_io_tree中有两个cache_tree,一个state，buffer。
	//我们现在已经知道了这个extent_state->state应该是记录了data metadata system之中的一个。
	//上面传过来的参数是data metadata system的或结果。用意显然就是为了找到一个extent就行。
	node = find_first_cache_extent(&tree->state, start);
	if (!node)
		goto out;

	while(1) {
		state = container_of(node, struct extent_state, cache_node);
		if (state->end >= start && (state->state & bits)) {//找到了非空的extent。
			*start_ret = state->start;
			*end_ret = state->end;
			ret = 0;
			break;
		}
		node = next_cache_extent(node);
		if (!node)
			break;
	}
out:
	return ret;
}


//我越发地觉得这个函数，实在冗长~~
//简而言之，这个函数就是通过对info block_group_cache 进行rb式的搜索，逐个group测试data bit位和剩余空间。
//返回找到组，类型btrfs_block_group_cache
struct btrfs_block_group_cache *btrfs_find_block_group(struct btrfs_root *root, struct btrfs_block_group_cache *hint, u64 search_start,
						 int data, int owner)
{
	struct btrfs_block_group_cache *cache;
	struct extent_io_tree *block_group_cache;
	struct btrfs_block_group_cache *found_group = NULL;
	struct btrfs_fs_info *info = root->fs_info;
	u64 used;
	u64 last = 0;
	u64 hint_last;
	u64 start;
	u64 end;
	u64 free_check;
	u64 ptr;
	int bit;
	int ret;
	int full_search = 0;
	int factor = 10;

	block_group_cache = &info->block_group_cache;

	if (!owner)
		factor = 10;

	bit = block_group_state_bits(data);

	if (search_start) {//这次是0
		struct btrfs_block_group_cache *shint;
		//之前的那个函数。
		shint = btrfs_lookup_block_group(info, search_start);
		if (shint && !shint->ro && block_group_bits(shint, data)) {
			//blcok group item
			used = btrfs_block_group_used(&shint->item);
			//pinned 在btrfs_read_block_groups设置为0.
			//key.offset is meaning of size.
			//if 是这个group尚有剩余空间。
			if (used + shint->pinned < div_factor(shint->key.offset, factor)) {
				return shint;
			}
		}
	}
	//这次调用hint也是NULL
	//bits测试很重要
	if (hint && !hint->ro && block_group_bits(hint, data)) {
		used = btrfs_block_group_used(&hint->item);
		//如果group尚有空间。
		if (used + hint->pinned < div_factor(hint->key.offset, factor)) {
			return hint;
		}
		last = hint->key.objectid + hint->key.offset;
		hint_last = last;
	} else {
		//如果提供了hint,btrfs_block_group_cache
		if (hint)
			hint_last = max(hint->key.objectid, search_start);
		else
			hint_last = search_start;

		last = hint_last;
	}
again:
	while(1) {
		ret = find_first_extent_bit(block_group_cache, last,
					    &start, &end, bit);
		if (ret)
			break;

		ret = get_state_private(block_group_cache, start, &ptr);
		if (ret)
			break;

		cache = (struct btrfs_block_group_cache *)(unsigned long)ptr;
		//new last~~
		last = cache->key.objectid + cache->key.offset;
		used = btrfs_block_group_used(&cache->item);
		//依靠data里面的bit位来确定我们需要哪个group。 
		if (!cache->ro && block_group_bits(cache, data)) {
			//随意检索？
			if (full_search)
				free_check = cache->key.offset;
			else
				free_check = div_factor(cache->key.offset,factor);
			if (used + cache->pinned < free_check) {
				//有剩余空间，满足
				found_group = cache;
				goto found;
			}
		}
		cond_resched();
	}
	if (!full_search) {
		last = search_start;
		full_search = 1;
		goto again;
	}
found:
	return found_group;
}

//在free_space_cahe 中记录空闲空间
//在从头至尾找到一个num大的空间 返回这个空间的start。
static int noinline find_search_start(struct btrfs_root *root,struct btrfs_block_group_cache **cache_ret,u64 *start_ret, int num, int data)
{
	int ret;
	struct btrfs_block_group_cache *cache = *cache_ret;
	u64 last;
	u64 start = 0;
	u64 end = 0;
	u64 search_start = *start_ret;
	int wrapped = 0;

	if (!cache) {
		goto out;
	}
again:
	//从函数名字可知就是把找到的这个group缓存到free space cache~~具体进去看看。
	ret = cache_block_group(root, cache);
	if (ret)
		goto out;

	last = max(search_start, cache->key.objectid);
	if (cache->ro || !block_group_bits(cache, data)) {
		goto new_group;
	}

	while(1) {
		ret = find_first_extent_bit(&root->fs_info->free_space_cache,last, &start, &end, EXTENT_DIRTY);
		if (ret) {
			goto new_group;
		}

		start = max(last, start);
		last = end + 1;
		if (last - start < num) {
			continue;
		}
		if (start + num > cache->key.objectid + cache->key.offset) {
			goto new_group;
		}
		*start_ret = start;
		return 0;
	}
out:
	cache = btrfs_lookup_block_group(root->fs_info, search_start);
	if (!cache) {
		printk("Unable to find block group for %llu\n",
			(unsigned long long)search_start);
		WARN_ON(1);
	}
	return -ENOSPC;

new_group:
	last = cache->key.objectid + cache->key.offset;
wrapped:
	cache = btrfs_lookup_first_block_group(root->fs_info, last);
	if (!cache) {
no_cache:
		if (!wrapped) {
			wrapped = 1;
			last = search_start;
			goto wrapped;
		}
		goto out;
	}
	cache = btrfs_find_block_group(root, cache, last, data, 0);
	cache = btrfs_find_block_group(root, cache, last, data, 0);
	if (!cache)
		goto no_cache;

	*cache_ret = cache;
	goto again;
}

//Just cache block group~~
static int cache_block_group(struct btrfs_root *root,struct btrfs_block_group_cache *block_group)
{
	struct btrfs_path *path;
	int ret;
	struct btrfs_key key;
	struct extent_buffer *leaf;
	struct extent_io_tree *free_space_cache;
	int slot;
	u64 last;
	u64 hole_size;

	if (!block_group)
		return 0;
	//we will record the blcok or extent in extent_root.
	root = root->fs_info->extent_root;
	//I think it also used to cache allocate.
	free_space_cache = &root->fs_info->free_space_cache;

	//already cached?
	if (block_group->cached)
		return 0;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	path->reada = 2;
	//以防万一
	last = max_t(u64, block_group->key.objectid, BTRFS_SUPER_INFO_OFFSET);
	//start address
	key.objectid = last;
	key.offset = 0;
	btrfs_set_key_type(&key, BTRFS_EXTENT_ITEM_KEY);
	//在extent_root中找到这个group，相近的item。
	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0)
		goto err;

	while(1) {
		leaf = path->nodes[0];
		slot = path->slots[0];
		//和下面的slots[0]++有关系，因为while的原因。
		if (slot >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(root, path);
			if (ret < 0)
				goto err;
			if (ret == 0) {
				continue;
			} else {
				break;
			}
		}
		得到我们找到的extent tree中的那个item的key。
		btrfs_item_key_to_cpu(leaf, &key, slot);
		//保证我们找到的item应该在我们想要的组里面。这个组是上面确定的。
		if (key.objectid < block_group->key.objectid) {
			goto next;
		}
		//超出了整个group的大小。
		if (key.objectid >= block_group->key.objectid +
		    block_group->key.offset) {
			break;
		}
		//进一步，验证这个item是否是一个extent item，因为从下面可知他是slots++ 一个个递加找到的，所以可能会遇到BLOCK GROUP item.
		if (btrfs_key_type(&key) == BTRFS_EXTENT_ITEM_KEY) {
			//如果是extent item ，计算和上一个item 之间的空间。
			if (key.objectid > last) {
				hole_size = key.objectid - last;
				//记录下这个空洞的大小到 info->free_space_cache->state ,cache_tree 类型。
				//通过rb node 实际上链入的extent_state
				set_extent_dirty(free_space_cache, last,last + hole_size - 1,GFP_NOFS);
			}
			last = key.objectid + key.offset;
		}
next:
		path->slots[0]++;
	}
	//最后一个item的end address 还在block group end address之内。
	if (block_group->key.objectid + block_group->key.offset > last) {
		//从最后一个item的结束位置到group结束的位置之间的空洞。
		hole_size = block_group->key.objectid + block_group->key.offset - last;
		//把这个空洞也记录到free space cache中。
		set_extent_dirty(free_space_cache, last, last + hole_size - 1, GFP_NOFS);
	}
	//我们先通过函数名猜功能，应该是从free space cache 中移除sb的内容
	remove_sb_from_cache(root, block_group);
	block_group->cached = 1;
err:
	btrfs_free_path(path);
	return 0;
}

static int remove_sb_from_cache(struct btrfs_root *root,struct btrfs_block_group_cache *cache)
{
	u64 bytenr;
	u64 *logical;
	int stripe_len;
	int i, nr, ret;
	struct extent_io_tree *free_space_cache;

	free_space_cache = &root->fs_info->free_space_cache;
	for (i = 0; i < BTRFS_SUPER_MIRROR_MAX; i++) {
		bytenr = btrfs_sb_offset(i);
		//mapping tree is just a encapsulation of cache_tree, simply, rb root
		//我想这个函数的主要功能时记录中的disk地址。
		//这个phusical 和logical中，logical 应是之加入raid 策略后的地址。
		ret = btrfs_rmap_block(&root->fs_info->mapping_tree,cache->key.objectid, bytenr, 0,&logical, &nr, &stripe_len);
		BUG_ON(ret);
		while (nr--) {
			clear_extent_dirty(free_space_cache, logical[nr],
				logical[nr] + stripe_len - 1, GFP_NOFS);
		}
		kfree(logical);
	}
	return 0;
}


//首先解析函数的参数的用途：
//map_tree:就是 fs_info的mapping_tree
//chunk_start:block group 的start位置，in disk。
//physical:btrfs sb 的offset 第一个64k，第二个是64M 第三个是128M。in disk。
//devid：0
//logical：return the logical address
//nraddrs：for return value，combined with logical，used to indicate array index
//stripe_len:for return value，条带长度。
//关于条带的几个参数含义仍然需要进一步，确定。
int btrfs_rmap_block(struct btrfs_mapping_tree *map_tree,u64 chunk_start, u64 physical, u64 devid,u64 **logical, int *naddrs, int *stripe_len)
{
	struct cache_extent *ce;
	struct map_lookup *map;
	u64 *buf;
	u64 bytenr;
	u64 length;
	u64 stripe_nr;
	int i, j, nr = 0;
	//
	ce = find_first_cache_extent(&map_tree->cache_tree, chunk_start);
	BUG_ON(!ce);
	
	map = container_of(ce, struct map_lookup, ce);

	//这个size应该是整个条带的大小，包括了作备份那部分重复的大小。
	length = ce->size;
	//raid 0 是striped，即num_stripes; raid1是备份,sub_stripes in raid10
	//length 是不考虑任何raid策略时，一次操作读写大小。
	if (map->type & BTRFS_BLOCK_GROUP_RAID10)
		length = ce->size / (map->num_stripes / map->sub_stripes);
	else if (map->type & BTRFS_BLOCK_GROUP_RAID0)
		length = ce->size / map->num_stripes;

	buf = kzalloc(sizeof(u64) * map->num_stripes, GFP_NOFS);

	for (i = 0; i < map->num_stripes; i++) {
		if (devid && map->stripes[i].dev->devid != devid)
			continue;
		//这个stripe是否包含super block in disk
		if (map->stripes[i].physical > physical ||
		    map->stripes[i].physical + length <= physical)
			continue;
		//superblock 所在的 stripe 编号或数量。
		stripe_nr = (physical - map->stripes[i].physical) /
			    map->stripe_len;

		if (map->type & BTRFS_BLOCK_GROUP_RAID10) {
			stripe_nr = (stripe_nr * map->num_stripes + i) /
				    map->sub_stripes;
		} else if (map->type & BTRFS_BLOCK_GROUP_RAID0) {
			stripe_nr = stripe_nr * map->num_stripes + i;
		}
		bytenr = ce->start + stripe_nr * map->stripe_len;
		for (j = 0; j < nr; j++) {
			if (buf[j] == bytenr)
				break;
		}
		if (j == nr)
			buf[nr++] = bytenr;
	}

	*logical = buf;
	*naddrs = nr;
	*stripe_len = map->stripe_len;

	return 0;
}


