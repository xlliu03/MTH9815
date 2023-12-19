

#ifndef DATA_GENERATOR_HPP
#define DATA_GENERATOR_HPP

#include <vector>
#include <map>
#include <string>
#include <random>
#include <iostream>
#include <fstream>
#include <boost/date_time.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;
using namespace boost::gregorian;

vector<string> g_product_Ids{ "OTRUSTR_02Y", "OTRUSTR_03Y","OTRUSTR_05Y","OTRUSTR_07Y",
				"OTRUSTR_10Y","OTRUSTR_20Y","OTRUSTR_30Y" };

map<string, string> g_tickers = { {"OTRUSTR_02Y","USB02Y"}, {"OTRUSTR_03Y","USB03Y"},
							{"OTRUSTR_05Y","USB05Y"}, {"OTRUSTR_07Y","USB07Y"}, {"OTRUS_10Y","USB10Y"},
							{"OTRUSTR_20Y","USB20Y"}, {"OTRUSTR_30Y","USB30Y"} };

map<string, double> g_coupons = { {"OTRUSTR_02Y",0.00375}, {"OTRUSTR_03Y",0.00625 },
							{"OTRUSTR_05Y",0.01500}, {"OTRUS_07Y",0.02250}, {"OTRUSTR_10Y",0.03125},
							{"OTRUSTR_20Y",0.03750}, {"OTRUS_30Y",0.04375} };

map<string, double> g_PV01s = { {"OTRUSTR_02Y",0.019851}, {"OTRUSTR_03Y",0.029309},
							{"OTRUSTR_05Y",0.048643}, {"OTRUSTR_07Y",0.065843}, {"OTRUSTR_10Y",0.087939},
							{"OTRUSTR_20Y",0.123655}, {"OTRUSTR_30Y",0.184696} };

map<string, date> g_dates = { {"OTRUSTR_02Y",date(2024,12,31)}, {"OTRUSTR_03Y",date(2025,12,31)},
							{"OTRUSTR_05Y",date(2027,12,31)}, {"OTRUSTR_07Y",date(2029,12,31)},
							{"OTRUSTR_10Y",date(2032,12,31)}, {"OTRUSTR_20Y",date(2042,12,31)},
							{"OTRUSTR_30Y",date(2052,12,31)} };

vector<string> books{ "TRSY1","TRSY2","TRSY3" };


// Generate trades.txt
void Generate_Trades(string file_name)
{
	boost::posix_time::ptime cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Generating trade data in " << file_name << "...\n";

	vector<string> sides{ "BUY","SELL" };
	ofstream out(file_name);
	int counter = 0;
	for (int i = 0; i < 7; ++i)
	{
		string productId = g_product_Ids[i];
		for (int j = 0; j < 10; ++j)
		{
			++counter;
			out << productId << ",TRADEID_";		
			if (counter < 10)
				out << "0" << to_string(counter) << ",";
			else
				out << to_string(counter) << ",";		
			out << books[(counter - 1) % 3] << ",";		

			int n1 = rand() % 2 + 99;
			int n2 = rand() % 32;
			int n3 = rand() % 8;
			string s1 = to_string(n1);
			string s2 = to_string(n2);
			string s3 = to_string(n3);
			if (n2 < 10)
				s2 = "0" + s2;
			if (n3 == 4)
				s3 = "+";
			string price = s1 + "-" + s2 + s3;
			out << price << ",";

			out << 1000000 * (j % 5 + 1) << "," << sides[j % 2] << endl;
		}
	}
	out.close();
	cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Trade data generated.\n\n";
}


// Generate prices.txt
void Generate_Prices(string file_name)
{
	boost::posix_time::ptime cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Generating price data in " << file_name << "...\n";

	ofstream out(file_name);
	for (int i = 0; i < 7; ++i)
	{
		string productId = g_product_Ids[i];
		for (int j = 0; j < 1000000; ++j)
		{
			out << productId << ",";
			
			int n1 = rand() % 2 + 99;
			int n2 = rand() % 32;
			int n3 = rand() % 8;
			double v1 = n1 + n2 * 1.0 / 32.0 + n3 * 1.0 / 252.0;
			string s1 = to_string(n1);
			string s2 = to_string(n2);
			string s3 = to_string(n3);
			if (n2 < 10)
				s2 = "0" + s2;
			if (n3 == 4)
				s3 = "+";
			string price1 = s1 + "-" + s2 + s3;

			n1 = rand() % 2 + 99;
			n2 = rand() % 32;
			n3 = rand() % 8;
			s1 = to_string(n1);
			s2 = to_string(n2);
			s3 = to_string(n3);
			if (n2 < 10)
				s2 = "0" + s2;
			if (n3 == 4)
				s3 = "+";
			double v2 = n1 + n2 * 1.0 / 32.0 + n3 * 1.0 / 252.0;
			string price2 = s1 + "-" + s2 + s3;
			
			if (v1 > v2)
				out << price2 << "," << price1 << endl;
			else
				out << price1 << "," << price2 << endl;

		}
		cur_time = boost::posix_time::microsec_clock::local_time();
		cout << cur_time << "  All prices generated for " << productId << endl;
	}
	out.close();
	cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Price data generated.\n\n";
}


// Generate marketdata.txt
void Generate_Mktdata(string file_name)
{
	boost::posix_time::ptime cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Generating order book data in " << file_name << "...\n";

	ofstream out(file_name);
	for (int i = 0; i < 7; ++i)
	{
		string productId = g_product_Ids[i];
		for (int j = 0; j < 1000000; ++j)
		{
			out << productId << ",";
			int n1, n2, n3;
			double v1, v2;
			string s1, s2, s3;
			for (int j = 1; j <= 5; ++j)
			{
				n1 = rand() % 2 + 99;
				n2 = rand() % 32;
				n3 = rand() % 8;
				v1 = n1 + n2 * 1.0 / 32.0 + n3 * 1.0 / 252.0;
				s1 = to_string(n1);
				s2 = to_string(n2);
				s3 = to_string(n3);
				if (n2 < 10)
					s2 = "0" + s2;
				if (n3 == 4)
					s3 = "+";
				string price1 = s1 + "-" + s2 + s3;

				n1 = rand() % 2 + 99;
				n2 = rand() % 32;
				n3 = rand() % 8;
				v2 = n1 + n2 * 1.0 / 32.0 + n3 * 1.0 / 252.0;
				s1 = to_string(n1);
				s2 = to_string(n2);
				s3 = to_string(n3);
				if (n2 < 10)
					s2 = "0" + s2;
				if (n3 == 4)
					s3 = "+";
				string price2 = s1 + "-" + s2 + s3;

				if (v1 > v2)
					out << price2 << "," << price1 << ",";
				else
					out << price1 << "," << price2 << ",";
			}
			out << endl;
		}
		cur_time = boost::posix_time::microsec_clock::local_time();
		cout << cur_time << "  All order book data generated for " << productId << endl;
	}
	out.close();
	cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Order book data generated.\n\n";
}


// Generate inquiries.txt
void Generate_Inquiry(string file_name)
{
	boost::posix_time::ptime cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Generating inquiry data in " << file_name << "...\n";

	vector<string> sides{ "BUY","SELL" };
	ofstream out(file_name);
	int counter = 0;
	for (int i = 0; i < 7; ++i)
	{
		string productId = g_product_Ids[i];
		for (int j = 0; j < 10; ++j)
		{
			++counter;
			out << productId << ",INQUIRYID_";
			if (counter < 10)
				out << "0" << to_string(counter) << ",";
			else
				out << to_string(counter) << ",";

			int n1 = rand() % 2 + 99;
			int n2 = rand() % 32;
			int n3 = rand() % 8;
			string s1 = to_string(n1);
			string s2 = to_string(n2);
			string s3 = to_string(n3);
			if (n2 < 10)
				s2 = "0" + s2;
			if (n3 == 4)
				s3 = "+";
			string price = s1 + "-" + s2 + s3;
			out << price << ",";

			out << 1000000 * (j % 5 + 1) << "," << sides[j % 2] << ",RECEIVED" << endl;
		}
	}
	out.close();
	cur_time = boost::posix_time::microsec_clock::local_time();
	cout << cur_time << "  Trade data generated.\n\n";
}

#endif