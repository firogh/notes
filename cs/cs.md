---
tags: [ cs ] 
title: Computer science a kernel programmer's perspective
date: 2015-02-27T15:46:14+08:00 
category: cs
---
# Reference
[Vijay Kumar B's Embedded Programming with the GNU Toolchain](http://www.bravegnu.org/gnu-eprog/)

# The Architecture of Computer science
SICP states that cs focuses on process.
What process is is the set of changes and quantaties.而这些changes 和 quantaties live in model of computation.
我们可以说一个process就是一个从属于model of computation的changes和quantaties的一次排列和集合.
所以model of computation至少是change的集合 和 quantaty的集合的集合.processess 本身就是暗含着data/quantaties的.
Why can  processes being? It's model of computation.
这就是cs的form, process和model of computation. What is the material of cs?
cs 的material 是symbolic.
For now, we have noticed that the form of cs is changes and quantities.
And the material of cs is symbolic(or, symbolic system).
Both form and material are properties of being/abyss.
人类的思维使用多种materials(vision, animation, graphics,or, feelings)去express a concept。
可以说程序设计语言就是标准描述 process的Semiotic system。
所以， pl对process的elements or origins做了合适的描述。
sicp研究的符号规则层面的process 和 data。 design pattern研究的是语义层面的。
PL 给了我们一种视角, 从process的元素的角度去描述process.
form language(BNF->syntax, regex->lexical)藐视pl的语法. 
form language可以通过automata鉴别.automata等价于form system lambda calculus.
自动机被相应的form grammar分类.
我们的目的是规范化人类思维的表达形式.以此来达到交流, 具现化.
我们要明白, pl的所有内容都是围绕the set of changes and quantaties建立的, 
也就是说pl是能指, 而所指便是the set of changes and quantaties.
syntax句法不是我们关注的, 我们关注的是semantic.从semantic角度去尽可能的描述changes和quantaty.
quantaties and changes: type system
我们给quantaties附上特定的类型, 表达特定的含义, 
primitive expression: 数学量, 符号学字符, function
combination: compound elements
abstraction: name an elements
relation: inference, relaton of changes, Flow Diagrams, Turing Machines And Languages With Only Two Formation Rules; 
也就是Jhone Locke的binary relation.
process 可以看成人的意志力展示.origins加入interence不关注changes. logic就是研究过程/changes和过程/changes之间的关系.
control flow描述设定的就是这种order/relation.核心不在于有没有if else, 而是是否有足够的表达能力.
表达changes之间的relation/order.可能是一种图的感觉, 就比如if else引起了tree这种order的产生.
也就是说语言的表达力. changes要类似automata的四通八达的感觉.
process由 changes, inference/relation, quantaties组成.因为这是由人的意志will 参与的process.
inference 强调的是order of changes/computation,也就是control flow.
Why does the order of execution matter?
Then we need programming:sicp c3(c2/c1)
then interpreter: sicp c4
link load
computation model: sicp c5
if else: 表示的是relation(changes之间的关系, order, causality, inference)
也就是说pl包含:
changes, quantaties, relation. 这几种表述.
pl和machine/model of computation近乎等价, progrm是machin的超集. program 是process.而pl不是.
所以pl和machine都对process有表达的能力.pl和machine的form一致, 但material不同. 他们都是process元素的集合.
programming是排列组合这些process elements的craft.可以说这些元素是data, 可以转变为process.
这里面我们就定义programmer的skills, crafts, 是一种排力组合的能力, 不严禁的通俗的理解.
design pattern 更注重宏观process. 他们排布process. 单这些process是处理别的process.high-order process(procedure?)
而algorithm, 可以认为是关注的是primitive/ process.
图灵机加入的"感知",正是laking of causal.
之于表述computational process, 认为(hypothesis?)自然语言和pl是具有相同能力的.
programming的目的之一是把思维结果转为pl形式表达出来.
另外, 我们现在能总结出, cs的两大主题就是:the form of process and how to express more better.
process的内在, 以及如何表达process.cs剩下的问题都是programmer 码农的问题.
现在我们来研究纯粹的process.继承sicp的定义.
procedure是process的体现. process >= procedure. 所谓的local evolution, 等价于c,lisp的function.
procedure 是一changes(quantaties, relation)有限的集合.
一个process可以包含多个procedure, 也可以只有一个procedure(如, 递归, 迭代).
还有另外一个就是data.process和data唇亡齿寒.
process: procedure, computation/changes.
data->: compound data, quantaty. 
我们研究的重点似乎时relation吧?
process和data既像一物两面, 又像唇亡齿寒.
下面开始, 
从combination, abstraction, relation这三个方面属性的process.
从简单到复杂, 先关注process.
如果process只有一个procedure.  elementary process.
似乎我们可以得到一个关于process的坐标系: procedure 的种类和数量.
这里, 我们不关注algorithm.
这里, 关注design pattern, GoF, elemental design pattern, POSA.
从坐标系角度只考虑process和process之间的关系:
1 只有一种quality的procedure, 完成的process.
那么数量上如果构成大于1, 也就是procedure call procedure.
可以是iteration or recursion; 我门的坐标系反应不出来.
需要另外一个属性, procedure 自身是否是运算的参数.
坐标系改下, procedure的数量换成procedure是否参与运算. 如果参与computation就是递归.
默认procedure 数量>=2.
我们再一次发现, procedures之间可能并不是直接调用的.
所以一个小坐标系, 能表达的太有限, 要废弃掉.从属性角度看.但可以明确procedures出现次数是2个以上.
当然, 我们是从process角度分析问题的.
只有一种procedure, 
* call/order relation
参与 computaton 是 recursive, otherwise, iteration.
iter(a,i, n)
if (i<n)
 a = a * (a +1)
 i++;
 return iter(a, i, n)//tail call>
看来这个问题的实质是process是否作为计算的对象.而recursive和iteration是某个general的特例.
loose relation: sequence(特例iteration), 是computation之间的关系.
strict relation: computation是内在的关系. computation被抽象data
primitive computation可以调function, function可以调primitive computation.
computation function 经过抽象变成data.data反过来又可构成computation.
可以说没有data, 全是computation!?那这么说(所谓的"recursive", 就司空见惯了.)
function反应的是computation 序列集合.自身调用自身, 首先满足这种 order sequence的关系.
似乎称之为recursive是没问题的. 但一个只能反应sequence, or和sequence等价的"recursive".
又有多大价值呢?recursive意义又有多大呢? 究竟, process的核心是什么呢? 是computation.
回答显示世界的recursive, 一个画面不断重复自身的画面!
![recursion](https://upload.wikimedia.org/wikipedia/commons/6/62/Droste.jpg)
我们发现, 他们总是在表达一个完整的个体.
画面的recursion, 依然在一个画面下.
function的recursion, 确实表现在一个function内.
但是从computation角度, recursive的function表述的是computation的连续sequence.
也就是说recursion是有对象的,对于process是computation是对象.
对于recursion process, 只有一个computation, 而iteration的process是多个computation的order/sequence集合.
所以要区分computation和procedure/function. function是computation和sequence集合.他反应的是computation之间的关系.
并不能反应computation自身内在关系.这也是我们混淆了recursion procedure的原因.
想想, 集合的复合还是computation sequence. 而computation自身的复合却是一个computation,自身的演变.
让我们体悟到了function和computation的区别.不同对象的交互operation差异.
对函数的讨论研究等价与, 讨论computation之间的关系.也就是把function全部替换成了computation.
所以recursive function就成了不断内嵌sequence computation了, 递归全无.
computation 内在的组合, 我们讨论了, 相似对象/function的情况.
如果一个computation内部,是由不同computation组成的.就是非递归的情况,edp叫Conglomeration.合理
computation内在讨论先停下.
我们关注computations之间的关系.
无论是computation的内部还是,computation之间data都成了纽带.
我们只讨论具有relation的process/computation.
如果通过共同的操作的data,
这就是并发的问题, 也就是changes.
如果共同的data, 不是作为操作的对象知识表达一个relation.
这是能想到的通过data的processes/computations之间的交互.
似乎量changes/computation/process 之间的relation, 只能是
causality and synchronization?(FIXME).
Causality: Observer(发起者主动, 结果者被动), polling(结果者主动)
state pattern(发起者的状态), Strategy Pattern(根据对象的选择行为)
Command Pattern(?decoupling, 貌似就是个Observer不过把状态给了接收者处理),
Mediator Pattern(decoupling), 
design pattern 关注的是form, syntax.
研究process/procedure/function/computation 到 data
 Simple Factory Pattern类似strategy pattern, 根据(接收者类型产生对象)
Factory Method Pattern类似(Mediator pattern, 将对象生成放给子类)
没有relation(or relationless)的process联系到一起.
adapter pattern
还有一种两个procedure concurrent, 要分开 order sequence化.
concurrent是design pattern等价的.
这样design pattern也融合进来了.
下面应该是SICP的chapter 3. chapter 3依然是对process的form的阐释和补全, 结合pl的语义进行挖掘.
另一方面结合具体的实现开始kernel相关的(可能涉及到hard ware), 分布式系统理解.
还有一个link load interpreter这方面的工作.

# programming 
Abstraction is vital in helping us to cope with the complexity of
large systems.
Effective program synthesis also requires organizational principles that
can guide us in formulating the overall design of a program.
orgnizational principles应该是architecture pattern.
我理解的programming的核心应该是modeling, modeling最直接的体现就是architecture pattern.
相较于design pattern, ap更注重整体?
不对, 这块不能把ap直接加进来, 逻辑不严密, 不完整.就先叫op吧.
why modular,we are not creating something.
It's just about the being.
abstraction is vital in helping us to cope with the complexity of
large systems. 通过抽象构造的material层级, 逐级清晰.
世界以我们所认为的那样方式运行着.
modular , that is, so that they can be divided “naturally” into co-
herent parts that can be separately developed and maintained.
I think it's because of the form.
model as modeled, action in local.
object-based approach and the stream-processing approach
objec is the form of this world. what about stream?

The difficulties of dealing with objects, change, and identity are a fundamental 
consequence of the need to grapple with time in our computational models.
Concurrent 增加了更多的难度, 但只是double的关系不是quality上的.

The stream approach can be most
fully exploited when we decouple simulated time in our model from the
order of the events that take place in the computer during evaluation.
We will accomplish this using a technique known as delayed evaluation .
stream 解耦了我模拟的时间和event的order.event有orderbut no time.
We don't care time. 隐约中我们引入了另一个origin of world, time.
我想还是需要关心时间的只是在stream的form里面不存在时间了.
leak of causal is abstraction.
second origin, state, An object is said to “have
state” if its behavior is influenced by its history.
We can characterize an object’s state by one or more state vari-
ables. 
有很多objects, 一些objects可能影响others's state, couple the state variables.
modular 需要decomposed into computational objects modeling actual objects.
model指整体如substitution 和environment.
Each computational object must have its own lo-
cal state variables describing the actual object’s state.
我们用computational object model actual, 每个 computational object must have its own local state variales.
local state variables 描述actual object's state. why local?

If we choose to model the flow of time in the system by the elapsed time
in the computer, then we must have a way to construct computational
objects whose behaviors change as our programs run. In particular, if
we wish to model state variables by ordinary symbolic names in the
programming language, then the language must provide an assignment
operator to enable us to change the value associated with a name.
从state variables 扯到 assignment operator.
因为state variables是记录actual objects的state的, state可能随时间改变.
我们必须改变state variables, 就需要assignment operator.表述一个关系, change.
一个对象自己的state variables 被称为 local?
形参? 
massage passing 类似strategy pattern 

From the point of view of one part of a complex process, the other
parts appear to change with time. They have hidden time-varying local
state. If we wish to write computer programs whose structure reflects
this decomposition, we make computational objects (such as bank ac-
counts and random-number generators) whose behavior changes with
time. We model state with local state variables, and we model the changes
of state with assignments to those variables.
整体上, a process, one part 看另外一面是
A process hide time-varying local state in modular, but with a changes behavior. encapuslation.
= changes, variable states

In general, programming with assignment forces us to carefully consider the relative orders
of the assignments to make sure that each statement is using the correct
version of the variables that have been changed. is issue simply does
not arise in functional programs.

函数调用对应的是substitution model
model of evaluation
environments
An environment is a sequence of frames . Each frame is a table (pos-
sibly empty) of bindings , which associate variable names with their cor-
responding values. (A single frame may contain at most one binding
for any variable.) Each frame also has a pointer to its enclosing environ-
ment , unless, for the purposes of discussion, the frame is considered to
be global . e value of a variable with respect to an environment is the
value given by the binding of the variable in the first frame in the en-
vironment that contains a binding for that variable. If no frame in the
sequence specifies a binding for the variable, then the variable is said to
be unbound in the environment.
所以programming实际讨论的是意义的问题.不是某种形式
3.4
e central issue lurking beneath the complexity of state, sameness,
and change is that by introducing assignment we are forced to admit
time into our computational models. Before we introduced assignment,
all our programs were timeless, in the sense that any expression that
has a value always has the same value.
so time is changing.
the execution of assignment
statements delineates
moments in time when values change. e result of evaluating an ex-
pression depends not only on the expression itself, but also on whether
the evaluation occurs before or aer these moments. Building models
in terms of computational objects with local state forces us to confront
time as an essential concept in programming.
we use assignmnet to record state/history.
We implemented the time variation of the states of the model objects in the computer
with assignments to the local variables of the model objects.
必须意识到change被modeled 到assignment.
model change in terms of sequences
list sequence is inefficient than the standard iterative style.
Streams are a clever idea that allows one to use sequence manipu-
lations without incurring the costs of manipulating sequences as lists.
With streams we can achieve the best of both worlds: We can formu-
late programs elegantly as sequence manipulations, while aaining the
efficiency of incremental computation.
stream 比list好,elegantly as sequence manipulation, efficiency as incremental computation.
(delay ⟨ exp ⟩ ) delayed object很有趣, 类似Brandon的enhanced greensight.连接是空, 穿越时空.

# Chapter 4 Metalinguistic abstraction
It is no exaggeration to regard this as the most fundamental idea in
programming:
e evaluator, which determines the meaning of expres-
sions in a programming language, is just another program.

SICP, 暂时看到这里.
programming 是modeling. 看了sicp差不多了.
interpreter: 
EOPL
PLAI
http://papl.cs.brown.edu/2016/
另外PL也到这里吧. 
source code写出来如何让run?
[弱类型、强类型、动态类型、静态类型语言的区别是什么？](https://www.zhihu.com/question/19918532)
c语言是弱类型语言的意思是：它的类型是给编译器看的，让编译器在初次分配内存的时候好分配一个指定大小的空间。在实际操作中你可以随意更改变量的类型（强制或自动）。
c语言实际是对内存直接操作的一门语言。也就是说如果给你四个字节的内存，你喜欢把它当成int来操作也行，当成四个char操作也行，随你喜欢。
[Is C strongly typed?](http://stackoverflow.com/questions/430182/is-c-strongly-typed)
I spent a few weeks, about a year ago, trying to sort out the terminology of "strongly typed," "statically typed," "safe," etc., and found it amazingly difficult. As your message points out, the usage of these terms is so various as to render them almost useless. --Benjamin C. Pierce
# program execution
run的目的是source code 对应到ISA.
最容易想到的就是compilation.
我们把compilation和interpreter当成从source code 到runnable program之间的某种form的变化.
先略过compilation, 也就是我们现在编译出了最简单形式的一个程序foo 输出hello world用到了一个新lib.
[How programs get run: ELF binaries](https://lwn.net/Articles/631631/)
我们知道了个大概, 那么我们最关心的事, 现在lib和foo都进入内存了, 那么foo是如何调用到lib的函数.
原来是直接利用address_space 最memory map private就共享了.
不需要显示维护library的list列表的.
这样程序就运行起来了.
硬件可以理解为programs的的interpreter. 理解为model of computations, 由set of changs and state/quantaties and relations 组成.
所以硬件也可以看成是抽象的! 只不过不能自动.
研究memory的是时候我们会区分到RAM和SRAM到电容和logic gates.也就是最终的实现.
这个时候, 我们发现material成了我们理解问题的所必须考虑的方面了.
也就是说理解到了电路这个层次, 我们就完成了standalone, Completeness的.
也就是hardware的material完成了我们的认知, 就是认知闭环了.
所以已经关注很多cpu的和process运行相关的指令, 对于理解lock锁(包括所谓的lockfree无锁)理解到硬件的material也是应该的.
为什么这么说? 如果不用关心到logic完整的material, 实际上我们不需要理解什么语言啊, computation model. 设计模式啊, 操作系统啊.
只要脑子想就够了.可是material也是being存在.也是有form的存在.
所以另外的, 很重要的就是, 我们不是理解hardware本身, 我们理解的是hardware背后的form!
form的内容就很少了.所以说学习要特别注意两点, form和completeness.
记住我们关心的不是hardware的material而是hardware的form.
所以说我们是为了完整complete form而不是material.
而实际上, 当我意识到所谓的form的存在的时候, hardware本身就成一种自然的延伸了.自然的被包括进学习的范围目标里面了(FIXME).

# os
所以下面的os, 从一个整体的角度去看待computer, 实质善顺道把os相关, 有助于os理解的hardware的知识
也包含进来. 下面的思考包含常见的GNU Linux/Unix/Windows这些系统, 和distribute system.
在更general 的form下思考他们的form.
我们似乎根本就不在乎是否有hardware参与进来, 我们关注的只是form. 如果form本身是完整的. hardware是不需要的.
为什么hardware对我们是不需要的, 因为我们所构建的cs的真正基础是纯粹逻辑的(FIXME), 可能有部分内容依赖物理如晶振, 电路的delta时间.
所以说, 我几乎不需要hardware.
开始分析os, os包括userspace, application. 睡觉. 
from this [ppt Last Class: OS and Computer Architecture](https://www.bowdoin.edu/~sbarker/teaching/courses/spring14/slides/lec03.pdf)
we know 

OS Service | Hardware Support 
------------|------------------------------------- 
Protection  | Kernel/user mode, protected instructions, base/limit registers 
Interrupts  | Interrupt vectors  
System calls  | Trap instructions and trap vectors  
I/O  | Interrupts and memory mapping  
Scheduling, error recovery,accounting  | Timer  
Synchronization  | Atomic instructions  
Virtual memory  | Translation look-aside buffers 
特别是synchronization和atomic instructions这条, 貌似冥冥中, 体现出了我们想要的感觉.
就我所知道的[几本OS的书](https://book.douban.com/people/firodb/all?sort=rating&start=0&tag=os&mode=grid&tags_sort=count), 基本都不符合我的思路. Operating Systems : Three Easy Pieces 这本还算可以吧.
有点心意, 但是达不到sicp的高度.
终于可以坐下来安心思考os了.上面的os书都是告诉你os有什么process, thread, memory management,
interrupt, IO等等, 而且讲的非常详细, 就好像他们真的是主角似得.
是的没错! os书里只关注这process, thread, scheduling, memory management, interrupt, IO之类的能干什么.
对, 尽量讲得很详细, 以为这样就好了. 可实际上呢? 学习者的思维, 全被那些符号填满, 同时思考的机会也
被剥夺了! 合理的os理解过程是什么样的呢? 我觉得还是从origins of world 往过来推导.
我们上来根本就不关心process 啊scheduling, memory management.IO, interrupt等等
我们最初有什么? 或者说, 我们正在做什么? 从origins 经过sicip.
只考虑最原始的情况, 为了完成computational process, 我们需要一个真实的model of computation.
我们有了他是relation(算术数学运算, order体现着结构上),  change(transfer, assignment), quantaties(state, memory)的集合.
我们为了, 完成一件事情编写了program开始运行. 似乎不太好想遇上了什么问题.
我们很容易想到让computer同时做量件事, 比如说两个人同时记日记.想当然, 我们也可以让一所学校的学生,
同时记日记.此时我们就意识到了, 要抽象下Process出来了, scheduling也来了. 同时我们也要统一管理set of states, memory了.
也有了IO, 我们要让内容输入进来, 显示出来.这都很好想. 
我们要保证系统有序的运行动起来.我设计了datapath和control unit.
现在我们往回解释,也就是从interrupt之类到origins, 讨论the form.
interrupt对应的是什么?是relation, 准确说是建立order.
interrupt为什么不能运行长时间, 当然可以运行长时间.
我们把interrupt, 看成和Process是一类东西, 都是process, change的实例.
memory和fs对state set的管理.所以至少process要和memory 和fs关联
貌似就没了?这只是框架.

form | os
-----|----
change, computation, transfer . | instructions(memory,IO), Process, interrupt
relation, order | synchronization/atomic, scheduling, interrupt
quantaties, state | fs, memory, device, driver


[test-and-set](http://web.cecs.pdx.edu/~walpole/class/cse513/slides/3.pdf)
test the lock, if false(nobody take it), then set the lock.
Test and test-and-set对TAS优化.
TAS bus lock, cache invalidation
TAS and TTAS is two different algorithms .
还需要面对contention, backoff and queue lock.
造成contention原因是什么, 就是cache.
因为lokc prefix的总线仲裁, 可以简单的按顺序来.
这样的话, concurrent, parallel涉及到的order就算是大概过了.
下面我们还是回到relation/order, change, quantaties上来.
这样我们用locality of reference and transfer/channel special/temporal.来解读了cache.
另外还有cache coherence.
我们把process, interrupt都看成computation.
而timeshareing 世界之源是作为process的基本属性, 解读了调度
memory subsystem 作为quantaties和state的管理比较直观.
fs和mm本质是一回事.
剩下一个IO, 就是transfer的实例化, 包括char, block, network device的驱动.
我们把peripheral device看成跟memory一样的东西也就是quantaties同时也涉及到了transfer
我们基本上把 整个os computer都包含进来了.
但还需要细节推理.
比如, fs:
我们要先有个文件file, 各种类型
mm 和fs都是quantaties的描述, 但根本却别在于fs是有人的参与的, 也就是说操作是要满足人的需求的.
这里fs包含了各种设备.mm是process和quantaties的关系. 而fs是人与quantaties的关系.
我们不考虑各种图形界面GUI, 只考虑最基础的linux console那种情况. 当然shell和linux console也要忽略掉.
从纯粹的使用场景出发, 我们是不关心什么具体的fs实现, 我们只关心具体的用户user如何操作几个or很多的文件.
最为真实的,实惠的, 基本的用户场景. 同样作为quantaties的mm是用数字标示quantaties的. 而且signifier根植
于电路版.而fs面对的对象是人, 而有自己的语言符号系统, 而且不能通过全部number记忆东西.
所以file要有个name, 文件多了, folder就有了, folder和folder之间就类似function之间调用.
也是体现的order.
我们发现folder和file这两个就能很好的描述用户使用了. 另外还有一个就是order, 不太被在意的.
现在fs就成了name的集合了并且那么之间都是线性的关系, 我们不考虑soft link.
我们来考虑下ln: a: hard link not allowed for directory. Why?
反证法如果我们允许hard link dir, 在不recursive情况下hard link ok. But 如果recursive怎么办?
soft link 也可能递归!所以recursive不是问题. 那么什么是
mkdir -p /tmp/a /tmp/a/b
如果
ln /tmp/a /tmp/la
怎么办, 就是递归.
ln /tmp/a/ /tmp/a/b/la1
toch /tmp/a/b/la1
貌似没理解到点子上.
现在用户哪里看到的就是多树.
开始fs.
有理由把dir和file看成一个东西.
下面file代表抽象了. file代表open, 就是用户面对的那个. 指向inode对应一个文件 inode连接address space backend.
那么问题来了, 就上面这个搓比的设计我们如何管理这些inode, file可以process弄个链表(大误, 数组)什么的好办.
如, struct file __rcu * fd-array[NR_OPEN_DEFAULT]; 这个可以理解. struct fdtable fdtab; 这个就不理解了.
不去管他. 我们如何组织inode, 比如我们打开是5本电子书, 7个文件, chrome, vim, 总之很多个文件, 那么这些inode怎么办?
最简单, 列个linked list就行.
in fs, namei stands for name interpreting,  the pathname lookup mechanisms (namei) by Feb-1997 T. Schoebel-Theuer
现在关于fs就make sense了.
关于open /dev, 参考dev_mount()._. ramfs_mknod vfs_mknode 
 drivers/base/devtmpfs.c <<handle_create>>
1. 从用户这个层面我们提供了dir 和 file, dir可以包含dir这三个concept涵盖住用户需求.

内核一个file 对应一次open, 先walk link path 之后do_last(walk和last交互着来 in wile()) 先去dcache 找dentry, 
(/dev 的dentry被pin住了, inode也建好了), 其他不在dcache找个empty dentry, 用父inode的iget之类操作
建个inode同时根据硬盘数据确定operation 类型.
我们发现:
2. file/ inode/ dentry是严格从属于fs的.
3. file -> dentry -> inode 这个顺序
4. inode决定了你的 fs之内的最高级别operations(如char ops). 进一步的(如tty 靠major minor)
5. special的inode不同于disk reglar 这种. 他们事先就created and pin dentry in core.
反观fs, sb和dentry是做骨架同inode是骨架的支撑, 不可缺少.
file是花拳绣腿. 有什么深刻的? 用户态松散的order和 transfer, quantaties 关系. dentry就是这种反应.
深刻1, 就是order 包含!
深刻2, 是什么, abstraction and function of everything(data, process)不同事物的抽象关系, 映射关系.
明天就是mm, 想对比fs关注与不同事物的组合, mm更纯粹, 也会涉及大quantaties的管理, 和disk之类应该有较大的相似性.
更能体现出quantaties.



can we open a file in interrupt?







# architecture of cs
Algorithm, TOC, Design Pattern, SICP, Logic, Mathematics
Programming: language, coding style
Compile, link, and load or interpret:
OS
Arch

* Top method
Abstruction
Combination
Virtualization
Exchange time and space
Isolation/Modular

* Top goal
Easy to use
Efficiency
Protection
Reliability
Security
Energy-efficiency

# Programming
Programming langueage: c, python, shell
Programming tools: vim
Compile Link: ELF
Testing
Debuging
Interface

# OS
Batch processing -> Time-sharing
Overlaying 
* vm
There are some great historical papers and books we should read before fully understanding virtual memory.
[Computer Structures: Readings and Examples ](http://research.microsoft.com/en-us/um/people/gbell/CGB%20Files/Computer%20Structures%20Readings%20and%20Examples%201971.pdf)
[Computer Structures: Readings and Examples html version](http://research.microsoft.com/en-us/um/people/gbell/Computer_Structures_Principles_and_Examples/contents.html)
Chapter 10 One-level storage system is the first implemention of virtual memory mind.
## Process management
进程的定义和PCB，进程和线程的区别，进程的三个基本状态及它们之间的转换关系，进程的同步，竞争和死锁，进程间通信
###Representation 
* Program memory
Stack(User/Kernel)
Heap
Data segment(data/bss)
Code segment
* PCB
Resource
Processor Context
Process state
###daemonize
http://fixunix.com/unix/84640-daemon-controlling-terminal.html
## Memory managerment
分页式管理，分段式管理，虚拟内存的概念，页面置换算法，内存分配算法
### Paging
paging is one of the memory management schemes by which 
a computer stores and retrieves data from the secondary storage for use in main memory.
* Page fault
###Page replacement algorithm
OPT
FIFO
Second-chance
LRU
### x86 memory segmentation
linux 基本不用
[Linux中的段](http://oss.org.cn/kernel-book/ch02/2.3.7.htm)
* GDT
* TSS
[Use of TSS in Linux](https://en.wikipedia.org/wiki/Task_state_segment#Use_of_TSS_in_Linux)
* Linear address
### [Virtual memory](https://en.wikipedia.org/wiki/Virtual_memory#Paged_virtual_memory)
It maps memory addresses used by a program, called virtual addresses, into physical addresses in computer memory.
* Logic/Virtual address
* Page table
### Memory allocation
* Buddy memory allocation. 
* Slab allocation/Memory Pool
## Device management
中断的概念，中断处理，I/O控制方式，缓冲区管理，设备驱动，磁盘调度和高速缓存
## Network stack
Protocol
## I/O
[Methods for designing a CPU's I/O interface generally fall into one of the following categories:](http://www.cs.uwm.edu/classes/cs458/Lecture/HTML/ch11s02.html)
Completely separate memory and I/O. buses DMA?
Common buses, separate control lines. Port-I/O
Common buses and control line. Memroy-maped I/O
###Higher level implemention of I/O
device or partion of device/memory -> file 
io -> stream
####[stream](https://en.wikipedia.org/wiki/Stream_(computing))
* [Standard streams](https://en.wikipedia.org/wiki/Standard_streams)
interface is stdio library or the kernel version.
* codata
### Low I/O type
* Programmed I/O/Polling
* DMA
* Interrupt I/O
* Channel I/O
### I/O scheduling
Elevator algorithm
###Asynchronous I/O NEED CLEAN
* synchronous I/O multiplexing and I/O event notification facility
select/poll/epoll
For the ease of use, the select loop is implemented as an *event loop* with callbacks.
libevent and libev is a well-designed *event loop*.Check shadowsocks for using of libev.
## File system 
文件的概念，文件的管理，文件系统
## System calls
系统调用的概念，系统调用的处理，系统调用类型
##CPU-device I/O
### Memory-mapped I/O
ioremap: physical address->logical address, simlar to vmalloc except we need not page.
### Ported-mapped I/O
##Non CPU-device I/O
### I/O channels
## 同步与异步IO
今天我们要辨析一下同步和异步IO. 我们先解释最基础的概念, 之后用生活化的例子
完成认知.
首先是blocking 和 non-blocking这两个概念. 这两个概念实质上是和IO没有关系.
他们是在说, 比如读数据, 如果没有数据我该怎么办. 也就是说, 他是在IO不存的时候,
在语义上才是有效, 如果你要读的数据始终存在, 那么你还会考虑阻塞与不阻塞的问题吗?
那你应该考虑什么? 同步还是异步IO, 倒地什么是同步或者异步呢?
英文synchronous, syn 和chronous构成, syn是在一起的意思而chronous是时间的意思.
也就是说在一个时间点上在一起, 那么是谁和谁在意一起呢?其中一个是IO可以肯定, 另外一个
就是执行IO的发起者, 通常也就是进程. 简单说来这个IO是由进程执行的.
那么异步IO呢, asynchronous是a + synchronous. a表否定, 我们知道在IO进行的过程中我们的
进程是始终存在的, 也就是说IO 和进程共享着相同的时间进度, 但是却不在一起.也就是说,
IO不是由我们的进程完成而是别的进程完成, 是谁呢,是内核线程.
那么我们就知道只有linux上的aio是符合异步IO的标准, 而多路复用, 如epoll返回是我们和IO是在
一起, 我们要调用read之类的完成他.
# Physical computation phenomenon
A Symbolic Analysis of Relay and Switching Circuits
The Mathematical Theory of Communication
Given a symbol level, the architecture is the description of the system in 
whatever system-description scheme exists next below the symbol level. - Newell, 1990, p. 81
[Ask HN: How to learn about the history of computing?](https://news.ycombinator.com/item?id=9844090)
* Control
Cogwheel control
electromechanical magnet plugging control
control sequence points


# Faq
* How does gcc attribute((aligned)) work?
struct S1 { short f; short f1; short f2;char a; char c;} __attribute__ ((aligned ));
sizeof S1 = 16 in 64-bit
* In what situation can unaligned accesss make a kernel panic?
may be arch/mips/kernel/unaligned.c
* Is the address generated by compiler physical or virtual?
Graphviz + CodeViz
