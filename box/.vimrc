"set mouse=r
"execute pathogen#infect()

"filetype plugin indent on
if has("cscope")
	set csprg=cscope
	set csto=0
	set cst
	set nocsverb
	" add any database in current diectory
	if filereadable("cscope.out")
		cs add cscope.out
	" else add DB pointed to by environmnent
	elseif $CSCOPE_DB != ""
		cs add $CSCOPE_DB
	endif
	set csverb
endif

set title
"autocmd BufRead * let &titlestring = expand("%:p")

"set t_Co=256
"let g:molokai_original=1
"syntax on
"hi cCustomFunc  gui=bold guifg=green
"hi cCustomClass gui=reverse guifg=#00FF00
set softtabstop=8
set tabstop=8
set shiftwidth=8
set noexpandtab
set number
"set autoindent
"set cindent
"set list
"set listchars=tab:▸\ ,eol:¬
map Y y$
"set mouse=a
imap jj <ESC>
map qq :q!<cr>
map -- :e#<cr>
map <c-l> :ts 
"map <c-b> :cs find g 
map <c-h> :cs find c <c-r><c-w><cr>
nmap <c-n><c-p> :Gtags -r <c-r><c-w><cr>
"nmap <c-n><c-p> :GtagsCursor<CR>
nmap <F9> :GtagsCursor<CR>
map <c-y> :Gtags -r
map <c-b> :cclose<cr>
map <c-j> :cn<cr>
map <c-k> :cp<cr>
"set fdm=syntax
":hi Folded guibg=blue guifg=blue ctermfg=blue ctermbg=none
":hi FoldColumn guibg=blue guifg=grey20 ctermfg=6 ctermbg=15
"set foldlevel=100
"nnoremap <space> @=((foldclosed(line('.')) < 0) ? 'zc' : 'zo')<CR> 
"map <F3> : call CompileGcc()<CR>
map <C-\> :ts <C-R><C-W><CR>
"map <c-m> :cs find g <c-r><c-w><cr>
"syntax enable
"set background=dark
"colorscheme solarized
"set term=screen-256color
"set term=xterm-256color
"let g:solarized_termcolors=256
"let g:solarized_contrast="low"
"let g:solarized_termcolors=16
"autocmd BufEnter *.note colorscheme	murphy

"au FileType c,cpp,sh,croql setlocal comments-=:/

"au FileType * setl fo-=cro


"Vundle
"set nocompatible    " be iMproved
"filetype off        " required!

"set rtp+=~/.vim/bundle/vundle/
"call vundle#rc()
" let Vundle manage Vundle
" " required!
"Bundle 'gmarik/vundle'
" " vim-scripts repos
"Bundle 'vim-plugin-foo'
"Bundle 'vim-plugin-bar'
"filetype plugin indent on    " required!


"au BufRead,BufNewFile *.md set filetype=markdown
"

"set nocompatible
"filetype off
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()
"Bundle 'gmarik/vundle'
Bundle 'Lokaltog/vim-powerline'
"filetype plugin indent on

set laststatus=2
"set t_Co=256
let g:Powerline_symbols = 'fancy'
"set encoding=utf-8
"set tw=80
"set fo=cqt
"set wm=0
"High light my personal note sytle --Firo
"highlight equalline cterm=bold ctermbg=none ctermfg=DarkMagenta
"match equalline /^=.*/

colorscheme molokai
"hi Comment guifg=#ABCDEF
highlight Comment ctermfg=blue
"http://www.color-hex.com/
"highlight sharpline cterm=bold ctermbg=none ctermfg=yellow
"2match sharpline /^#.*/

"highlight starline cterm=bold ctermbg=none ctermfg=green
"let m = matchadd("starline", '^\*.*')
"3match subline /^\*.*/
"
nnoremap <C-^> :bn<CR>
nnoremap <C-6> :bp<CR>
"set columns=80

let @j = "⊃"
let @k = "∪"
let @l = "→"
let @h = "∅" 
let @i = "⊢"
