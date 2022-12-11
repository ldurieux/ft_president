#include "ft_president.h"

s_ft_president president;

using namespace std;

struct VectorHashFunction
{
	size_t operator()(const vector<uint64_t>& groups) const
	{
		size_t res = 0;

		for (uint64_t i = 0; i < groups.size(); i++)
			res += groups[i] << i;
		return res;
	}
};

typedef unordered_set<uint64_t> GroupSet;
typedef unordered_set<uint64_t>::const_iterator GroupSetIt;
typedef unordered_set<vector<uint64_t>, VectorHashFunction> Set;
typedef unordered_set<vector<uint64_t>, VectorHashFunction>::const_iterator SetIt;

GroupSet groups;
GroupSetIt groupsEnd;
Set sets;

uint64_t	idx_to_bin(uint8_t idx)
{
	uint64_t res = 1;

	res <<= idx;
	return (res);
}

void	recursive_compute_groups(uint8_t depth, uint64_t arrPts, uint8_t ptIdx)
{
	if (depth == 0)
	{
		const auto it = groups.find(arrPts);
		if (it == groups.end())
			groups.insert(arrPts);
		return ;
	}

	const uint8_t *neighbors = &president.neighbors[ptIdx][0];
	while (*neighbors != 255)
	{
		uint64_t tmpArrPts = arrPts | ((uint64_t)1 << *neighbors);
		if (tmpArrPts != arrPts)
			recursive_compute_groups(depth - 1, tmpArrPts, *neighbors);
		neighbors++;
	}
}

void	compute_groups()
{
	for (uint8_t depth = 0; depth < president.count - president.res_size + 1; depth++)
		for (uint8_t idx = 0; idx < president.count; idx++)
			recursive_compute_groups(depth, (uint64_t)1 << idx, idx);
}

void	recursive_compute_sets(uint64_t goalMask, GroupSetIt begin, uint64_t arrPts, int8_t groupCount,
							   uint64_t *curArr)
{
	if (arrPts == goalMask)
	{
		if (groupCount != president.res_size)
			return ;
		*curArr = *begin;
		curArr -= groupCount - 1;
		vector<uint64_t> vec(groupCount);
		for (int i = 0; i < president.res_size; i++)
			vec.push_back(curArr[i]);
		sets.insert(vec);
		return ;
	}
	if (begin == groupsEnd || groupCount == president.res_size)
		return ;

	*curArr = *begin;
	curArr++;
	begin++;
	for (auto it = begin; it != groupsEnd; ++it)
	{
		if (arrPts & *it)
			continue ;
		uint64_t tmpArrPts = arrPts | *it;
		recursive_compute_sets(goalMask, it, tmpArrPts, groupCount + 1, curArr);
	}
}

void	compute_sets(uint8_t goalPts)
{
	uint64_t	goalMask = 0;
	for (uint64_t i = 0; goalPts != 0; i++, goalPts--)
		goalMask |= (uint64_t)1 << i;

	uint64_t count = 0;
	uint64_t *tmpArr = new uint64_t[president.res_size];
	for (auto it = groups.begin(); it != groupsEnd; it++)
	{
		recursive_compute_sets(goalMask, it, *it, 1, tmpArr);
		count++;
		if (count % 64 == 0)
			cerr << "calculing sets... " << (float)count / (float)groups.size() * 100.0f << "%" << endl;
	}
}

uint64_t getSum(uint64_t pts)
{
	uint64_t sum = 0;

	for (uint64_t i = 0; pts != 0; i++)
	{
		if (!(pts & ((uint64_t)1 << i)))
			continue ;
		sum += president.pibs[i];
		pts -= (uint64_t)1 << i;
	}
	return (sum);
}

vector<uint64_t>	compute_res()
{
	int64_t goal = (int64_t)president.mean * (int64_t)president.mean;
	vector<uint64_t> bestGroup;
	int64_t bestDiff = INT64_MAX;

	for (const auto& curGroup : sets)
	{
		int64_t val = 0;
		for (uint64_t i : curGroup)
		{
			int64_t sum = getSum(i);
			val += (president.mean - sum) * (president.mean - sum);
		}
		val = abs(goal - (int64_t)(val / curGroup.size()));
		if (val < bestDiff)
		{
			bestDiff = val;
			bestGroup = curGroup;
		}
	}
	return bestGroup;
}

void	print_res(const vector<uint64_t>& bestGroup)
{
	for (unsigned long it : bestGroup)
	{
		bool first = true;
		for (uint64_t i = 0; i < 64; i++)
		{
			if (it & ((uint64_t)1 << i))
			{
				if (!first)
					cout << '-';
				cout << president.names[i];
				first = false;
			}
		}
		cout << endl;
	}
}

void	solve(t_ft_president *_president)
{
	president = *_president;

	compute_groups();
	cerr << "group count: " << groups.size() << endl;

	compute_sets(president.count);
	cerr << "set count: " << sets.size() << endl;

	vector<uint64_t> bestGroup = compute_res();

	print_res(bestGroup);
	cerr << "done!" << endl;
}
