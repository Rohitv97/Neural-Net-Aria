// RoNet.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//
#include "pch.h"
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<complex>
#include<vector>
#include<fstream>
#include<math.h>
using namespace std;

const int hidden_neurons = 3;
//const double lambda = 0.6;
//const double eta = 0.65;
//const double alpha = 0.1;

class Neuron
{
public:
	int layer;
	double lambda;
	double eta;
	double alpha;
	//double lambda = 0.6;
	double w0;
	double w1[3];
	double w2[hidden_neurons + 1];
	double in1;
	double netIn = 0;
	double output;
	double local_gradient;
	//double eta = 0.65;
	double deltaweight1[3];
	double deltaweight2[hidden_neurons + 1];
	//double alpha = 0.1;

	void initWeight(int layer, int hidden_neurons)
	{
		float X = 1;
		if (layer == 1)
		{
			for (int i = 0; i <= 2; i++)
			{
				float r1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));
				w1[i] = r1 - 0;
				deltaweight1[i] = 0;
				cout << "Weights of hidden layer: " << w1[i] << endl;
			}
		}
		else if (layer == 2)
		{
			for (int i = 0; i <= hidden_neurons; i++)
			{
				float r1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / X));
				w2[i] = r1 - 0;
				deltaweight2[i] = 0;
				cout << "Weights of output layer: " << w2[i] << endl;
			}
		}
		else
		{
			w0 = 0;
		}

	}

	double netInput(vector<Neuron*> prevLayer, int layer)
	{
		double netIn = 0;
		if (layer == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				//cout << "Previous layer " << i << " output: " << prevLayer.at(i)->output << endl;
				netIn = netIn + (w1[i] * prevLayer.at(i)->output);
			}
			netIn = netIn + w1[2];
		}
		else if (layer == 2)
		{
			for (int i = 0; i < hidden_neurons; i++)
			{
				//cout << "Previous layer " << i << " output: " << prevLayer.at(i)->output << endl;
				netIn = netIn + (w2[i] * prevLayer.at(i)->output);
			}
			netIn = netIn + w2[hidden_neurons];
		}
		else
		{
			netIn = in1;
		}
		//cout << "Net Input of Neuron: " << netIn << endl;

		return netIn;
	}

	double activation(double nIn, int layer)
	{
		if (layer == 0)
		{
			output = nIn;
			return output;
		}
		else
		{
			double z = lambda * nIn;
			output = 1 / (1 + (exp(-z)));
			return output;
		}
	}

	void Input()
	{
		cout << "Enter input: ";
		cin >> in1;
	}

	void gradient(int layer, double error, vector<Neuron*> nextLayer, int neuron)
	{
		double sum_of_gw = 0.0;
		if (layer == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				sum_of_gw += nextLayer.at(i)->w2[neuron] * nextLayer.at(i)->local_gradient;
			}
			local_gradient = lambda * output * (1 - output) * sum_of_gw;
			//cout << "Sum of (Gradient of next * Weight) :  " << sum_of_gw << endl;
			//cout << "Local gradient of hidden neuron " << neuron << " : " << local_gradient << endl;
		}
		else if (layer == 2)
		{
			local_gradient = error * output * (1 - output) * lambda;
			//cout << "Local gradient of output neuron " << neuron << " : " << local_gradient << endl;
		}
	}

	void weightUpdate(int layer, vector<Neuron*> prevLayer, vector<Neuron*> biasLayer)
	{
		if (layer == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				deltaweight1[i] = (eta * local_gradient * prevLayer[i]->output) + (alpha * deltaweight1[i]);
				w1[i] += deltaweight1[i];

				//cout << "Delta W of hidden neurons: " << deltaweight1[i] << endl;
				//cout << "New weight : " << w1[i] << endl;

			}
			deltaweight1[2] = (eta * local_gradient * biasLayer[0]->output) + (alpha * deltaweight1[2]);
			w1[2] += deltaweight1[2];
		}
		else if (layer == 2)
		{
			for (int i = 0; i < hidden_neurons; i++)
			{
				deltaweight2[i] = (eta * local_gradient * prevLayer[i]->output) + (alpha * deltaweight2[i]);
				w2[i] += deltaweight2[i];

				//cout << "Delta W of output neurons: " << deltaweight2[i] << endl;
				//cout << "New weight : " << w2[i] << endl;

			}
			deltaweight2[hidden_neurons] = (eta * local_gradient * biasLayer[1]->output) + (alpha * deltaweight2[hidden_neurons]);
			w2[hidden_neurons] += deltaweight2[hidden_neurons];
		}
	}
};


vector<vector<double>> readCSV()
{
	vector<double> row;
	vector <vector<double>> alldata;

	ifstream file("nntrainingdata.csv");
	string line, word;
	getline(file, line);

	while (getline(file, line))
	{
		row.clear();

		stringstream ss(line);

		while (getline(ss, word, ','))
		{
			row.push_back(stod(word));
		}

		alldata.push_back(row);
	}

	return alldata;
}

void model_save(vector<Neuron*> hidLayer, vector<Neuron*> outLayer)
{
	ofstream model("model.txt");
	if (model.is_open())
	{
		for (unsigned int i = 0; i < hidLayer.size(); i++)
		{
			for (int j = 0; j <= 2; j++)
			{
				double weight = hidLayer[i]->w1[j];
				model << weight << "\n";
			}
		}

		for (unsigned int i = 0; i < outLayer.size(); i++)
		{
			for (int j = 0; j <= hidden_neurons; j++)
			{
				double weight = outLayer[i]->w2[j];
				model << weight << "\n";
			}
		}
		model.close();
	}
	cout << "Model has been exported" << endl;
}

void model_load(vector<Neuron*> hidLayer, vector<Neuron*> outLayer)
{
	ifstream model("model.txt");
	string line;
	double value;

	for (unsigned int i = 0; i < hidLayer.size(); i++)
	{
		for (int j = 0; j < 2; j++)
		{
			getline(model, line);
			value = stod(line);
			hidLayer[i]->w1[j] = value;
			//cout << hidLayer[i]->w1[j] << endl;

		}
	}

	for (unsigned int i = 0; i < outLayer.size(); i++)
	{
		for (int j = 0; j < hidden_neurons; j++)
		{
			getline(model, line);
			value = stod(line);
			outLayer[i]->w2[j] = value;
			//cout << outLayer[i]->w2[j] << endl;
		}
	}
}


int main()
{
	srand(static_cast <unsigned> (time(0)));
	double x1n, x2n, h1n, y1n, y2n;
	double x1o, x2o, h1o, y1o, y2o;

	Neuron x1, x2, y1, y2;

	vector<Neuron*> inLayer;
	inLayer.push_back(&x1);
	inLayer.push_back(&x2);

	vector<Neuron*> outLayer;
	outLayer.push_back(&y1);
	outLayer.push_back(&y2);

	vector<Neuron*> hidLayer;
	for (int i = 0; i < hidden_neurons; i++)
	{
		Neuron* h = new Neuron();
		hidLayer.push_back(h);
	}

	Neuron b1, b2;
	vector<Neuron*> biasLayer;
	biasLayer.push_back(&b1);
	biasLayer.push_back(&b2);

	x1.initWeight(0, 3);
	x2.initWeight(0, 3);
	b1.initWeight(0, 3);
	b2.initWeight(0, 3);

	b1.output = 1;
	b2.output = 1;

	for (unsigned int i = 0; i < hidLayer.size(); i++)
	{
		hidLayer[i]->initWeight(1, 3);
	}

	y1.initWeight(2, 3);
	y2.initWeight(2, 3);

	vector<vector<double>> alldata;
	alldata = readCSV();

	double temp_e1, temp_e2;

	for (double L = 0.3; L < 1; L = L + 0.2)
	{
		for (double E = 0.4; E < 1; E = E + 0.2)
		{
			for (double A = 0.1; A < 1; A = A + 0.2)
			{

				for (unsigned int i = 0; i < inLayer.size(); i++)
				{
					inLayer[i]->lambda = L;
					inLayer[i]->eta = E;
					inLayer[i]->alpha = A;
				}

				for (unsigned int i = 0; i < outLayer.size(); i++)
				{
					outLayer[i]->lambda = L;
					outLayer[i]->eta = E;
					outLayer[i]->alpha = A;
				}

				for (unsigned int i = 0; i < hidLayer.size(); i++)
				{
					hidLayer[i]->lambda = L;
					hidLayer[i]->eta = E;
					hidLayer[i]->alpha = A;
				}

				for (unsigned int i = 0; i < biasLayer.size(); i++)
				{
					biasLayer[i]->lambda = L;
					biasLayer[i]->eta = E;
					biasLayer[i]->alpha = A;
				}

				string filename = "model_" + to_string(L) + "_" + to_string(E) + "_" + to_string(A) + ".txt";
				vector<double> overallErr;

				for (int epoch = 0; epoch < 100; epoch++)
				{
					double err1 = 0.0, err2 = 0.0;
					double me1 = 0.0, me2 = 0.0;

					for (unsigned int i = 0; i < alldata.size(); i++)
					{
						//min range = 0, max range = 5000
						x1.in1 = (alldata[i][0] / 5000);
						x2.in1 = (alldata[i][1] / 5000);

						double t1, t2;

						//min speed = 0, max speed = 250
						t1 = ((alldata[i][2]) / 250);
						t2 = ((alldata[i][3]) / 250);

						x1n = x1.netInput(inLayer, 0);
						x2n = x2.netInput(inLayer, 0);
						x1o = x1.activation(x1n, 0);
						x2o = x2.activation(x2n, 0);

						/*
						cout << "Output of x1: " << x1o << endl;
						cout << "Output of x2: " << x2o << endl;
						*/

						for (unsigned int j = 0; j < hidLayer.size(); j++)
						{
							h1n = hidLayer[j]->netInput(inLayer, 1);
							h1o = hidLayer[j]->activation(h1n, 1);
							//cout << "Output of h" << i+1 << ": " << h1o << endl;
						}

						y1n = y1.netInput(hidLayer, 2);
						y2n = y2.netInput(hidLayer, 2);
						y1o = y1.activation(y1n, 2);
						y2o = y2.activation(y2n, 2);

						/*
						cout << "Output of y1: " << y1o << endl;
						cout << "Output of y2: " << y2o << endl;
						*/

						temp_e1 = ((t1 - y1o) * (t1 - y1o));
						me1 += temp_e1;
						err1 = (t1 - y1o);
						//cout << "Error 1: " << err1 << endl;
						y1.gradient(2, err1, outLayer, 0);

						temp_e2 = ((t2 - y2o) * (t2 - y2o));
						me2 += temp_e2;
						err2 = (t2 - y2o);
						//cout << "Error 2: " << err2 << endl;
						y2.gradient(2, err2, outLayer, 1);

						double err_h = 0.0;

						for (unsigned int j = 0; j < hidLayer.size(); j++)
						{
							hidLayer[j]->gradient(1, err_h, outLayer, j);
						}

						b1.gradient(1, err_h, hidLayer, 0);
						b2.gradient(1, err_h, outLayer, 1);

						y1.weightUpdate(2, hidLayer, biasLayer);
						y2.weightUpdate(2, hidLayer, biasLayer);

						for (unsigned int j = 0; j < hidLayer.size(); j++)
						{
							hidLayer[j]->weightUpdate(1, inLayer, biasLayer);
						}

					}

					me1 = me1 / alldata.size();
					me2 = me2 / alldata.size();
					me1 = sqrt(me1);
					me2 = sqrt(me2);

					double error = ((me1)+(me2)) / 2;

					cout << "Epoch " << epoch + 1 << " error: " << error << endl;
					overallErr.push_back(error);

				}

				ofstream file(filename);
				if (file.is_open())
				{
					for (int num = 0; num < overallErr.size(); num++)
					{
						file << overallErr[num] << "\n";
					}
					file.close();
					overallErr.clear();
				}

			}
		}
	}
	

	

	//model_save(hidLayer, outLayer);

	inLayer.clear();
	hidLayer.clear();
	outLayer.clear();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
