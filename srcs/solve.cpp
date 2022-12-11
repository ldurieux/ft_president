#include "ft_president.h"

s_ft_president president;

using namespace std;

struct Group
{
	Group(uint64_t pts) :
		points(pts)
	{
	}

	void compute()
	{
		sum = getSum(points);
	}

	uint64_t	points;
	uint64_t	sum;

	bool operator==(const Group& other) const
	{
		return points == other.points;
	}

	struct HashFunction
	{
		size_t operator()(const Group& group) const { return group.points; }
	};

	struct SetHashFunction
	{
		size_t operator()(const vector<Group>& groups) const
		{
			size_t res = 0;

			for (uint64_t i = 0; i < groups.size(); i++)
				res += groups.at(i).points << i;
			return res;
		}
	};

private:
	static uint64_t getSum(uint64_t pts)
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
};

typedef unordered_set<Group, Group::HashFunction> GroupSet;
typedef unordered_set<Group, Group::HashFunction>::const_iterator GroupSetIt;
typedef unordered_set<vector<Group>, Group::SetHashFunction> Set;
typedef unordered_set<vector<Group>, Group::SetHashFunction>::const_iterator SetIt;

GroupSet groups;
GroupSetIt groupsEnd;
Set sets;

uint64_t	idx_to_bin(uint8_t idx)
{
	uint64_t res = 1;

	res <<= idx;
	return (res);
}

void	recursive_compute_groups(int8_t depth, uint64_t arrPts, uint8_t ptIdx)
{
	if (depth == 0)
	{
		Group group(arrPts);
		const auto it = groups.find(group);
		if (it == groups.end())
			groups.insert(group);
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
							   vector<Group> &curArr)
{
	if (groupCount > president.res_size)
		return ;
	if (arrPts == goalMask)
	{
		if (groupCount != president.res_size)
			return ;
		curArr.push_back(*begin);
		const auto it = sets.find(curArr);
		if (it == sets.end())
			sets.insert(curArr);
		return ;
	}
	if (begin == groupsEnd)
		return ;

	curArr.push_back(*begin);
	begin++;
	for (GroupSetIt it = begin; it != groupsEnd; it++)
	{
		if (arrPts & it->points)
			continue ;
		uint64_t tmpArrPts = arrPts | it->points;
		vector<Group> cpyArr = curArr;
		recursive_compute_sets(goalMask, it, tmpArrPts, groupCount + 1, cpyArr);
	}
}

void	compute_sets(uint8_t goalPts)
{
	uint64_t	goalMask = 0;
	for (uint64_t i = 0; goalPts != 0; i++, goalPts--)
		goalMask |= (uint64_t)1 << i;

	uint64_t count = 0;
	for (auto it = groups.begin(); it != groupsEnd; it++)
	{
		vector<Group> tmpArr;
		recursive_compute_sets(goalMask, it, it->points, 1, tmpArr);
		count++;
		if (count % 64 == 0)
			cerr << "calculing sets... " << (float)count / (float)groups.size() * 100.0f << "%" << endl;
	}
}

vector<Group>	compute_res()
{
	int64_t goal = (int64_t)president.mean * (int64_t)president.mean;
	vector<Group> bestGroup;
	int64_t bestDiff = INT64_MAX;

	for (auto it = sets.begin(); it != sets.end(); it++)
	{
		vector<Group> curGroup = *it;
		int64_t val = 0;
		for (int i = 0; i < curGroup.size(); i++)
		{
			curGroup[i].compute();
			val += (president.mean - (int64_t)curGroup[i].sum) * (president.mean - (int64_t)curGroup[i].sum);
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

void	print_res(const vector<Group>& bestGroup)
{
	for (auto it = bestGroup.begin(); it != bestGroup.end(); it++)
	{
		bool first = true;
		for (uint64_t i = 0; i < 64; i++)
		{
			if (it->points & ((uint64_t)1 << i))
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

	vector<Group> bestGroup = compute_res();

	print_res(bestGroup);
	cerr << "done!" << endl;

	return ;
}
