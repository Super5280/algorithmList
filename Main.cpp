#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

struct Sample   // 样本
{
	int id = 0;
	double feature[2] = { 0.0 };
};

struct Cluster_center  // 聚类中心
{
	Sample scenter;
	std::vector<int> sam_ids;
};

class Classifier
{
public:
	Classifier(std::string path);
	void k_means(int k);
private:
	void print(Cluster_center* pts, int k);
	std::vector<Sample> sample;
};

Classifier::Classifier(std::string path)
{
	std::ifstream ifile(path, std::ios::in);
	if (ifile.is_open())
	{
		int count = 0;
		while (!ifile.eof())
		{
			Sample s;
			s.id = count;
			ifile >> s.feature[0] >> s.feature[1];
			sample.push_back(s);
			count++;
		}
	}
	else
	{
		std::cout << "文件打开失败！\n" << std::endl;
		exit(-3);
	}
}

void Classifier::k_means(int k)
{
	// 选择初始聚类中心
	Cluster_center* pts = new Cluster_center[k];
	Cluster_center* old_pts = new Cluster_center[k];
	for (int i = 0; i < k; i++)
	{
		(pts + i)->scenter = sample.at(i);
		(pts + i)->sam_ids.push_back(i);
	}
	int status = 1;
	while (status)
	{
		for (int i = 0; i < k; i++)
		{
			(pts + i)->sam_ids.clear();
		}
		// 循环遍历求距离,进行分类
		for (int i = 0; i < sample.size(); i++)
		{
			std::vector<double> distance;
			double dis;
			for (int j = 0; j < k; j++)
			{
				dis = sqrt(pow((pts + j)->scenter.feature[0] - sample[i].feature[0], 2) +
					pow((pts + j)->scenter.feature[1] - sample[i].feature[1], 2));
				distance.push_back(dis);
			}
			// 最小值所对应的下标
			int min_index = min_element(distance.begin(), distance.end()) - distance.begin();

			((pts + min_index)->sam_ids).push_back(i);  // 将该样本压入对应的聚类中心里面去
		}

		for (int i = 0; i < k; i++)
		{
			(old_pts + i)->scenter.feature[0] = (pts + i)->scenter.feature[0];
			(old_pts + i)->scenter.feature[1] = (pts + i)->scenter.feature[1];
		}

		// 更新聚类中心
		for (int i = 0; i < k; i++)
		{
			int num;
			double sumx[2] = { 0.0 };
			for (int j = 0; j < (pts + i)->sam_ids.size(); j++)
			{
				num = (pts + i)->sam_ids[j];
				sumx[0] += sample[num].feature[0];
				sumx[1] += sample[num].feature[1];
			}
			(pts + i)->scenter.feature[0] = sumx[0] / (pts + i)->sam_ids.size();
			(pts + i)->scenter.feature[1] = sumx[1] / (pts + i)->sam_ids.size();
		}
		std::vector<double> e;
		for (int i = 0; i < k; i++)
		{
			double e1 = (old_pts + i)->scenter.feature[0] - (pts + i)->scenter.feature[0];
			e.push_back(abs(e1));
			double e2 = (old_pts + i)->scenter.feature[1] - (pts + i)->scenter.feature[1];
			e.push_back(abs(e2));
		}
		if (*max_element(e.begin(), e.end()) < 1e-9)
			status = 0;
	}
	print(pts, k);
	delete[]pts;
	delete[]old_pts;
}

void Classifier::print(Cluster_center* pts, int k)
{
	for (int i = 0; i < k; i++)
	{
		std::cout << "==========第" << i + 1 << "类:  ==========" << std::endl;
		for (int j = 0; j < (pts + i)->sam_ids.size(); j++)
		{
			std::cout << "(" << sample[(pts + i)->sam_ids[j]].feature[0] << ","
				<< sample[(pts + i)->sam_ids[j]].feature[1] << ")" << " ";
		}
		std::cout << "\n";
	}
}

int main()
{
	std::string path = "data.txt";
	Classifier c(path);
	c.k_means(2);
	return 0;
}