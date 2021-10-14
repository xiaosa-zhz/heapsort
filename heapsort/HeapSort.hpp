#pragma once
//在_CompareImpl处调整排序方向

template<typename data_t, data_t... nums>
struct _NumSeq
{
	using type = data_t;
	constexpr static size_t size = sizeof...(nums);
};

template<typename data_t>
struct _NumSeq<data_t>
{
	using type = void;
	constexpr static size_t size = 0;
};

template<typename data_t>
using _EmptyNumSeq = _NumSeq<data_t>;

//返回下标为sub的值的traits

template<size_t num>
struct _EqualZero { constexpr static bool value = false; };

template<>
struct _EqualZero<0> { constexpr static bool value = true; };

template<size_t sub, typename NumSeq, bool zero_flag>
struct _Sub;

template<size_t sub, typename data_t, data_t numnow, data_t... nums>
struct _Sub<sub, _NumSeq<data_t, numnow, nums...>, false>
{
	constexpr static data_t value = _Sub<sub - 1, _NumSeq<data_t, nums...>, _EqualZero<sub - 1>::value>::value;
};

template<size_t sub, typename data_t, data_t numnow, data_t... nums>
struct _Sub<sub, _NumSeq<data_t, numnow, nums...>, true>
{
	constexpr static data_t value = numnow;
};

//合并_NumSeq的traits

template<typename... NumSeq>
struct _NumSeqMerge;

template<typename data_t, data_t... nums1, data_t... nums2>
struct _NumSeqMerge<_NumSeq<data_t, nums1...>, _NumSeq<data_t, nums2...>>
{
	using result = _NumSeq<data_t, nums1..., nums2...>;
};

template<typename NumSeq1, typename NumSeq2, typename... NumSeqRest>
struct _NumSeqMerge<NumSeq1, NumSeq2, NumSeqRest...>
{
	using result = typename _NumSeqMerge<typename _NumSeqMerge<NumSeq1, NumSeq2>::result, NumSeqRest...>::result;
};

//切分_NumSeq的traits，从下标sub处切开后下标为sub的元素在second的下标0处（即第一个）

template<size_t sub, typename NumSeq1, typename NumSeq2, bool zero_flag>
struct _NumSeqSplitImpl;

template<size_t sub, typename NumSeq1, typename NumSeq2>
struct _NumSeqSplitImpl<sub, NumSeq1, NumSeq2, true>
{
	using first = NumSeq1;
	using second = NumSeq2;
};

template<size_t sub, typename data_t, data_t next_move_num, data_t... nums1, data_t... nums2>
struct _NumSeqSplitImpl<sub, _NumSeq<data_t, nums1...>, _NumSeq<data_t, next_move_num, nums2...>, false>
{
	using NextSplit = _NumSeqSplitImpl<sub - 1, _NumSeq<data_t, nums1..., next_move_num>, _NumSeq<data_t, nums2...>, _EqualZero<sub - 1>::value>;
	using first = typename NextSplit::first;
	using second = typename NextSplit::second;
};

template<size_t sub, typename NumSeq>
struct _NumSeqSplit;

template<size_t sub, typename data_t, data_t... nums>
struct _NumSeqSplit<sub, _NumSeq<data_t, nums...>>
{
	using origin = _NumSeq<data_t, nums...>;
	using result = _NumSeqSplitImpl<sub, _EmptyNumSeq<data_t>, origin, _EqualZero<sub>::value>;
	using first = typename result::first;
	using second = typename result::second;
};

//交换下标为Sub1和Sub2的元素

template<size_t sub1, size_t sub2, typename NumSeq>
struct _Exchange;

template<size_t sub, typename data_t, data_t... nums>
struct _Exchange<sub, sub, _NumSeq<data_t, nums...>>
{
	//下标相同的情况不交换
	using origin = _NumSeq<data_t, nums...>;
	constexpr static bool exchange_flag = false;
	using result = origin;
};

template<size_t sub1, size_t sub2, typename data_t, data_t... nums>
struct _Exchange<sub1, sub2, _NumSeq<data_t, nums...>>
{
	//将原始序列切成5段，然后拼接为交换后的版本
	using origin = _NumSeq<data_t, nums...>;
	constexpr static bool exchange_flag = true;
	//第一次切分，得到sub1之前的部分
	using firstcut = _NumSeqSplit<sub1, origin>;
	using head = typename firstcut::first;
	using firstcut_rest = typename firstcut::second;
	//第二次切分，切下sub1
	using secondcut = _NumSeqSplit<1, firstcut_rest>;
	using sub1elem = typename secondcut::first;
	using secondcut_rest = typename secondcut::second;
	//第三次切分，切下sub1和sub2中间的部分，注意此时原sub2的下标是sub2-sub1-1
	using thirdcut = _NumSeqSplit<sub2 - sub1 - 1, secondcut_rest>;
	using mid = typename thirdcut::first;
	using thirdcut_rest = typename thirdcut::second;
	//第四次切分也是最后一次，将sub2和余下部分分开
	using forthcut = _NumSeqSplit<1, thirdcut_rest>;
	using sub2elem = typename forthcut::first;
	using tail = typename forthcut::second;
	//以交换后顺序合并5段
	using result = typename _NumSeqMerge<head, sub2elem, mid, sub1elem, tail>::result;
};

//比较sub1，sub2和sub3三个元素哪个最大，返回其下标

template<typename data_t, data_t value1, data_t value2>
struct _CompareImpl
{
	constexpr static bool result = value1 > value2;
};

template<bool result, size_t sub1, size_t sub2, typename data_t, data_t value1, data_t value2>
struct _Compare;

template<size_t sub1, size_t sub2, typename data_t, data_t value1, data_t value2>
struct _Compare<true, sub1, sub2, data_t, value1, value2>
{
	constexpr static size_t result_sub = sub1;
	constexpr static data_t result = value1;
};

template<size_t sub1, size_t sub2, typename data_t, data_t value1, data_t value2>
struct _Compare<false, sub1, sub2, data_t, value1, value2>
{
	constexpr static size_t result_sub = sub2;
	constexpr static data_t result = value2;
};

template<size_t sub1, size_t sub2, size_t sub3, typename data_t, data_t value1, data_t value2, data_t value3>
struct _TriCompareImpl
{
	using firstcompare = _Compare<_CompareImpl<data_t, value1, value2>::result, sub1, sub2, data_t, value1, value2>;
	constexpr static size_t temp_result_sub = firstcompare::result_sub;
	constexpr static data_t temp_result = firstcompare::result;
	using secondcompare = _Compare<_CompareImpl<data_t, temp_result, value3>::result, temp_result_sub, sub3, data_t, temp_result, value3>;
	constexpr static size_t result_sub = secondcompare::result_sub;
	constexpr static data_t result = secondcompare::result;
};

template<size_t sub1, size_t sub2, size_t sub3, typename NumSeq>
struct _TriCompare;

template<size_t sub1, size_t sub2, size_t sub3, typename data_t, data_t... nums>
struct _TriCompare<sub1, sub2, sub3, _NumSeq<data_t, nums...>>
{
	using origin = _NumSeq<data_t, nums...>;
	constexpr static data_t value1 = _Sub<sub1, origin, _EqualZero<sub1>::value>::value;
	constexpr static data_t value2 = _Sub<sub2, origin, _EqualZero<sub2>::value>::value;
	constexpr static data_t value3 = _Sub<sub3, origin, _EqualZero<sub3>::value>::value;
	constexpr static size_t result = _TriCompareImpl<sub1, sub2, sub3, data_t, value1, value2, value3>::result_sub;
};

//叶子情况分类器

template<bool left_leaf_out, bool right_leaf_out>
struct _LeafOutImpl;

template<>
struct _LeafOutImpl<false, false>
{
	constexpr static size_t leaf_out_flag = 0;
};

template<>
struct _LeafOutImpl<false, true>
{
	constexpr static size_t leaf_out_flag = 1;
};

template<>
struct _LeafOutImpl<true, true>
{
	constexpr static size_t leaf_out_flag = 2;
};

template<size_t sub, typename NumSeq>
struct _LeafOut;

template<size_t sub, typename data_t, data_t... nums>
struct _LeafOut<sub, _NumSeq<data_t, nums...>>
{
	using origin = _NumSeq<data_t, nums...>;
	constexpr static size_t leaf_out_flag = _LeafOutImpl<((sub * 2 + 1) >= origin::size), ((sub * 2 + 2) >= origin::size)>::leaf_out_flag;
};

//维护堆的向下调整操作

template<bool exchange_flag, size_t leaf_out_flag, size_t sub, typename NumSeq>
struct _AdjustDown;

//两个子节点都存在的情况
template<size_t sub, typename data_t, data_t... nums>
struct _AdjustDown<true, 0, sub, _NumSeq<data_t, nums...>>
{
	//进行三者比较并递归向下调整
	using origin = _NumSeq<data_t, nums...>;
	constexpr static size_t newrootsub = _TriCompare<sub, sub * 2 + 1, sub * 2 + 2, origin>::result;
	using exchange = _Exchange<sub, newrootsub, origin>;
	using exchange_result = typename exchange::result;
	using result = typename _AdjustDown<exchange::exchange_flag, _LeafOut<newrootsub, exchange_result>::leaf_out_flag, newrootsub, exchange_result>::result;
};

//只有左子节点存在的情况
template<size_t sub, typename data_t, data_t... nums>
struct _AdjustDown<true, 1, sub, _NumSeq<data_t, nums...>>
{
	//只与左子节点进行比较并尝试调整
	using origin = _NumSeq<data_t, nums...>;
	constexpr static data_t value_sub = _Sub<sub, origin, _EqualZero<sub>::value>::value;
	constexpr static data_t value_left = _Sub<sub * 2 + 1, origin, _EqualZero<sub * 2 + 1>::value>::value;
	using compare = _Compare<_CompareImpl<data_t, value_sub, value_left>::result, sub, sub * 2 + 1, data_t, value_sub, value_left>;
	constexpr static size_t newrootsub = compare::result_sub;
	using exchange = _Exchange<sub, newrootsub, origin>;
	using exchange_result = typename exchange::result;
	using result = typename _AdjustDown<exchange::exchange_flag, _LeafOut<newrootsub, exchange_result>::leaf_out_flag, newrootsub, exchange_result>::result;
};

//没有子节点，即本节点是叶子节点的情况
template<size_t sub, typename NumSeq>
struct _AdjustDown<true, 2, sub, NumSeq>
{
	//什么都不做
	using origin = NumSeq;
	using result = origin;
};

//上一级没有发生调整的情况，不继续调整
template<size_t leaf_out_flag, size_t sub, typename NumSeq>
struct _AdjustDown<false, leaf_out_flag, sub, NumSeq>
{
	using result = NumSeq;
};

//建堆操作

template<size_t sub, typename NumSeq, bool zero_flag>
struct _MakeHeapImpl;

template<size_t sub, typename data_t, data_t... nums>
struct _MakeHeapImpl<sub, _NumSeq<data_t, nums...>, false>
{
	using origin = _NumSeq<data_t, nums...>;
	using adjust_result = typename _AdjustDown<true, _LeafOut<sub, origin>::leaf_out_flag, sub, origin>::result;
	using result = typename _MakeHeapImpl<sub - 1, adjust_result, _EqualZero<sub - 1>::value>::result;
};

template<size_t sub, typename data_t, data_t... nums>
struct _MakeHeapImpl<sub, _NumSeq<data_t, nums...>, true>
{
	using origin = _NumSeq<data_t, nums...>;
	using result = typename _AdjustDown<true, _LeafOut<sub, origin>::leaf_out_flag, sub, origin>::result;
};

template<typename NumSeq>
struct _MakeHeap;

template<typename data_t, data_t... nums>
struct _MakeHeap<_NumSeq<data_t, nums...>>
{
	using origin = _NumSeq<data_t, nums...>;
	using result = typename _MakeHeapImpl<(origin::size / 2) - 1, origin, _EqualZero<(origin::size / 2) - 1>::value>::result;
};

//堆排序

template<size_t remain_pop, bool zero_flag, typename NumSeq>
struct _HeapPop;

template<size_t remain_pop, typename data_t, data_t... nums>
struct _HeapPop<remain_pop, false, _NumSeq<data_t, nums...>>
{
	//还有剩余的未出堆元素，将堆顶元素出堆然后向下调整
	using origin = _NumSeq<data_t, nums...>;
	using exchange = _Exchange<0, remain_pop, origin>;
	using exchange_result = typename exchange::result;
	using split_result = _NumSeqSplit<remain_pop, exchange_result>;
	using remain_heap = typename split_result::first;
	using finished_seq = typename split_result::second;
	using adjust = _AdjustDown<true, _LeafOut<0, remain_heap>::leaf_out_flag, 0, remain_heap>;
	using adjusted_remain_heap = typename adjust::result;
	using poped_seq = typename _NumSeqMerge<adjusted_remain_heap, finished_seq>::result;
	using result = typename _HeapPop<remain_pop - 1, _EqualZero<remain_pop - 1>::value, poped_seq>::result;
};

template<size_t remain_pop, typename data_t, data_t... nums>
struct _HeapPop<remain_pop, true, _NumSeq<data_t, nums...>>
{
	using origin = _NumSeq<data_t, nums...>;
	using result = origin;
};

template<typename data_t, data_t... nums>
struct HeapSort
{
	using origin = _NumSeq<data_t, nums...>;
	using heap = typename _MakeHeap<origin>::result;
	using result = typename _HeapPop<origin::size - 1, _EqualZero<origin::size - 1>::value, heap>::result;
};

template<typename data_t, data_t... nums>
using HeapSortResult = typename HeapSort<data_t, nums...>::result;