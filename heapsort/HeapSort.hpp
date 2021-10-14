#pragma once
//��_CompareImpl������������

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

//�����±�Ϊsub��ֵ��traits

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

//�ϲ�_NumSeq��traits

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

//�з�_NumSeq��traits�����±�sub���п����±�Ϊsub��Ԫ����second���±�0��������һ����

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

//�����±�ΪSub1��Sub2��Ԫ��

template<size_t sub1, size_t sub2, typename NumSeq>
struct _Exchange;

template<size_t sub, typename data_t, data_t... nums>
struct _Exchange<sub, sub, _NumSeq<data_t, nums...>>
{
	//�±���ͬ�����������
	using origin = _NumSeq<data_t, nums...>;
	constexpr static bool exchange_flag = false;
	using result = origin;
};

template<size_t sub1, size_t sub2, typename data_t, data_t... nums>
struct _Exchange<sub1, sub2, _NumSeq<data_t, nums...>>
{
	//��ԭʼ�����г�5�Σ�Ȼ��ƴ��Ϊ������İ汾
	using origin = _NumSeq<data_t, nums...>;
	constexpr static bool exchange_flag = true;
	//��һ���з֣��õ�sub1֮ǰ�Ĳ���
	using firstcut = _NumSeqSplit<sub1, origin>;
	using head = typename firstcut::first;
	using firstcut_rest = typename firstcut::second;
	//�ڶ����з֣�����sub1
	using secondcut = _NumSeqSplit<1, firstcut_rest>;
	using sub1elem = typename secondcut::first;
	using secondcut_rest = typename secondcut::second;
	//�������з֣�����sub1��sub2�м�Ĳ��֣�ע���ʱԭsub2���±���sub2-sub1-1
	using thirdcut = _NumSeqSplit<sub2 - sub1 - 1, secondcut_rest>;
	using mid = typename thirdcut::first;
	using thirdcut_rest = typename thirdcut::second;
	//���Ĵ��з�Ҳ�����һ�Σ���sub2�����²��ַֿ�
	using forthcut = _NumSeqSplit<1, thirdcut_rest>;
	using sub2elem = typename forthcut::first;
	using tail = typename forthcut::second;
	//�Խ�����˳��ϲ�5��
	using result = typename _NumSeqMerge<head, sub2elem, mid, sub1elem, tail>::result;
};

//�Ƚ�sub1��sub2��sub3����Ԫ���ĸ���󣬷������±�

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

//Ҷ�����������

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

//ά���ѵ����µ�������

template<bool exchange_flag, size_t leaf_out_flag, size_t sub, typename NumSeq>
struct _AdjustDown;

//�����ӽڵ㶼���ڵ����
template<size_t sub, typename data_t, data_t... nums>
struct _AdjustDown<true, 0, sub, _NumSeq<data_t, nums...>>
{
	//�������߱Ƚϲ��ݹ����µ���
	using origin = _NumSeq<data_t, nums...>;
	constexpr static size_t newrootsub = _TriCompare<sub, sub * 2 + 1, sub * 2 + 2, origin>::result;
	using exchange = _Exchange<sub, newrootsub, origin>;
	using exchange_result = typename exchange::result;
	using result = typename _AdjustDown<exchange::exchange_flag, _LeafOut<newrootsub, exchange_result>::leaf_out_flag, newrootsub, exchange_result>::result;
};

//ֻ�����ӽڵ���ڵ����
template<size_t sub, typename data_t, data_t... nums>
struct _AdjustDown<true, 1, sub, _NumSeq<data_t, nums...>>
{
	//ֻ�����ӽڵ���бȽϲ����Ե���
	using origin = _NumSeq<data_t, nums...>;
	constexpr static data_t value_sub = _Sub<sub, origin, _EqualZero<sub>::value>::value;
	constexpr static data_t value_left = _Sub<sub * 2 + 1, origin, _EqualZero<sub * 2 + 1>::value>::value;
	using compare = _Compare<_CompareImpl<data_t, value_sub, value_left>::result, sub, sub * 2 + 1, data_t, value_sub, value_left>;
	constexpr static size_t newrootsub = compare::result_sub;
	using exchange = _Exchange<sub, newrootsub, origin>;
	using exchange_result = typename exchange::result;
	using result = typename _AdjustDown<exchange::exchange_flag, _LeafOut<newrootsub, exchange_result>::leaf_out_flag, newrootsub, exchange_result>::result;
};

//û���ӽڵ㣬�����ڵ���Ҷ�ӽڵ�����
template<size_t sub, typename NumSeq>
struct _AdjustDown<true, 2, sub, NumSeq>
{
	//ʲô������
	using origin = NumSeq;
	using result = origin;
};

//��һ��û�з������������������������
template<size_t leaf_out_flag, size_t sub, typename NumSeq>
struct _AdjustDown<false, leaf_out_flag, sub, NumSeq>
{
	using result = NumSeq;
};

//���Ѳ���

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

//������

template<size_t remain_pop, bool zero_flag, typename NumSeq>
struct _HeapPop;

template<size_t remain_pop, typename data_t, data_t... nums>
struct _HeapPop<remain_pop, false, _NumSeq<data_t, nums...>>
{
	//����ʣ���δ����Ԫ�أ����Ѷ�Ԫ�س���Ȼ�����µ���
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