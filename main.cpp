#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string.h>
#include "functions.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
using namespace std;

vector<float> read_file(std::string s)
{
    try
    {
        vector<float> v;
        std::ifstream infile;
        infile.open(s);
        if(!infile)
        {
            throw "Error in file opening, kindly check the path of the file given";
        }
        std::string sd;
        float data;
        while(getline(infile, sd))
        {
            data=stof(sd);

            v.push_back(data);
        }
        return v;
    }
    catch (std::invalid_argument)
    {
        throw "Either the data in your file is not valid or you have given a larger number of rows.";
    }
    catch (const char* msg)
    {
        throw msg;
    }
}

vector<float> sigmoid(vector<float> v)
{
    vector<float> ans;
    int size=static_cast<int>(v.size());
    for(int i=0; i<size; i++)
    {
        double t1=exp(v[i]*(-1));
        double t2=1/(1+t1);
        float t3=(float)(t2);
        ans.push_back(t3);
    }
    return ans;
}

vector<float> softmax(vector<float> v)
{
    vector<float> ans;
    vector<float> mid;
    double sum=0;
    int size=static_cast<int>(v.size());
    for(int i=0; i<size; i++)
    {
        double t1=exp(v[i]);
        mid.push_back((float)(t1));
        sum=sum+t1;
    }

    for(int i=0; i<size; i++)
    {
        float t2=(float)(mid[i]/sum);
        ans.push_back(t2);
    }
    return ans;

}

vector<vector<float> > tanh(vector<vector<float> > m)
{
    int n=static_cast<int>(m.size());
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            float t1=m[i][j];
            double t2=exp(t1);
            double t3=exp(t1*(-1));
            float t4=(float)((t2-t3)/(t2+t3));
            m[i][j]=t4;
        }
    }
    return m;
}

vector<vector<float> > relu(vector<vector<float> > m)
{
    int n=static_cast<int>(m.size());
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            float t1=m[i][j];
            if (t1<0)
            {
                m[i][j]=0;
            }
        }
    }
    return m;
}

vector<vector<float> > maxpooling(vector<vector<float> > m, int width, int stride=1)
{
    int n=static_cast<int>(m.size());
    float t1=((n-width)%stride);
    if (t1!=0)
    {
        throw "The stride and width given by you are not an appropriate choice";
    }

    int rows=0;
    while ((width+rows*stride)<=n)
    {
        rows+=1;
    }

    vector<vector<float> > ans;
    for (int i=0; i<rows; i++)
    {
        vector<float> v(rows,0);
        ans.push_back(v);
    }

    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<rows; j++)
        {
            float max=m[i*stride][j*stride];
            for (int iw=i*stride; iw<i*stride+width; iw++)
            {
                for (int jw=j*stride; jw<j*stride+width; jw++)
                {
                    float temp=m[iw][jw];
                    if (max<temp)
                    {
                        max=temp;
                    }
                }
            }
            ans[i][j]=max;
        }
    }
    return ans;
}

vector<vector<float> > average_pooling(vector<vector<float> > m, int width, int stride=1)
{
    int n=static_cast<int>(m.size());
    float t1=((n-width)%stride);
    if (t1!=0)
    {
        std::cout<<"The stride and width given by you are not an appropriate choice, still the output will be"<<std::endl;
    }
    float square=width*width;
    int rows=0;
    while ((width+rows*stride)<=n)
    {
        rows+=1;
    }

    vector<vector<float> > ans(rows,vector<float>(rows,0));
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<rows; j++)
        {
            float sum=0;
            for (int iw=i*stride; iw<i*stride+width; iw++)
            {
                for (int jw=j*stride; jw<j*stride+width; jw++)
                {
                    float temp=m[iw][jw];
                    sum=sum+temp;
                }
            }
            float average=sum/square;
            ans[i][j]=average;
        }
    }
    return ans;

}

vector<vector<float> > reflect_kernel(vector<vector<float> > kernel)
{
    float temp;
    vector<vector<float> > reflected_kernel = kernel;
    int rows = static_cast<int>(kernel.size());
    for(int i=0;i<rows;i++)
    {
        for(int j=i;j<rows;j++)
        {
            temp = reflected_kernel[i][j];
            reflected_kernel[i][j]=reflected_kernel[j][i];
            reflected_kernel[j][i]=temp;
        }
    }

    for(int i=0; i<rows;i++)
    {
        for(int j=rows - i - 1;j<rows;j++)
        {
            temp = reflected_kernel[i][j];
            reflected_kernel[i][j] = reflected_kernel[rows - j - 1][rows - i - 1];
            reflected_kernel[rows - j-1][rows - i-1] = temp;
        }
    }

    return reflected_kernel;
}

vector<vector<float> > convolve(vector<vector<float> > matrix, vector<vector<float> > kernel)
{
    vector<vector<float> > reflected_kernel = reflect_kernel(kernel);
    int m =static_cast<int>( matrix.size());
    int k = static_cast<int>(kernel.size());
    vector < vector <float> > convoluted_matrix(m-k+1, std::vector<float>(m-k+1, 0));

    int x, y, i, j;
    float sum;

    for(x=0;x<m-k+1;x++)
    {
        for(y=0;y<m-k+1;y++)
        {
            sum=0;
            for(i=x;i<x+k;i++)
            {
                for(j=y;j<y+k;j++)
                {
                    sum = sum + reflected_kernel[i-x][j-y]*matrix[i][j];
                }
            }
            convoluted_matrix[x][y]=sum;
        }
    }
    return convoluted_matrix;
}

vector<vector<float> > padding(vector<vector<float> > matrix, int pad)
{
    int m = static_cast<int>(matrix.size());
    int dim = m+(2*pad);
    vector < vector <float> > padded_matrix(dim, std::vector<float>(dim, 0));

    for(int i=0; i<dim; i++)
    {
        for(int j=0; j<dim; j++)
        {
            if((i>=pad && i<=pad+m-1) && (j>=pad && j<=pad+m-1))
            {
                padded_matrix[i][j]=matrix[i-pad][j-pad];
            }
        }
    }
    return padded_matrix;
}

vector<vector<float> > matrix_multiply(vector<vector<float> > mat1, vector<vector<float> > mat2)
{
    if(mat2.size()==mat1[0].size())
    {
        vector<vector<float> > result(mat1.size(), vector<float>(mat2[1].size(),0));
        for(int i=0;i<mat1.size();i++)
        {
            for(int j=0;j<mat2[1].size();j++)
            {
                for(int k=0;k<mat2.size();k++)
                    result[i][j]=result[i][j]+(mat1[i][k]*mat2[k][j]);
            }
        }
        return result;
    }
    else
    {
        throw "Matrix Dimensions don't match!";
    }
}

struct multiple_return_variables
{
    vector < vector <float> > matrix;
    std::chrono::duration<double> elapsed;
};

vector<vector<float>> toeplitz_convolve(vector<vector<float> > matrix, vector<vector<float> > kernel, int option)
{
    int m = static_cast<int>(matrix.size());
    int k = static_cast<int>(kernel.size());
    int dim = m+k-1;
    vector<vector<float> > padded_kernel(dim, vector<float>(dim,0));

    for(int i = 0; i<k; i++)
    {
        for(int j=0; j<k; j++)
        {
            padded_kernel[i][j]=kernel[i][j];
        }
    }

    vector<vector<vector<float> > > h(dim,vector<vector<float> >(dim,vector<float>(m,0)));

    for(int z=0; z<dim; z++)
    {
        vector<float> a = padded_kernel[z];
        int i=0;
        for(int y=0; y<m; y++)
        {
            for (int x = i; x < dim; x++)
            {
                h[z][x][y]=a[x-i];
            }
            i++;
        }
    }

    vector<vector<float> > h_2d(dim*dim, vector<float>(m*m,0));
    int b=0;//Poorva-I don't know if it is to be made float or not
    for(int i=0; i<dim*dim; i+=dim)
    {
        b=i/dim;
        for(int j=0;j<m*m; j+=m)
        {
            if(b<0)
            {
                b=dim-1;
            }
            for(int p=i;p<i+dim;p++)
            {
                for(int l=j;l<j+m;l++)
                {
                    h_2d[p][l]=h[b][p-i][l-j];
                }
            }
            b--;
        }
    }

    vector<vector<float> > resized_input(m*m,vector<float>(1,0));
    int p=0;
    for(int i=0; i<m; i++)
    {
        for(int j=0; j<m; j++)
        {
            resized_input[p][0]=matrix[i][j];
            p++;
        }
    }
    vector<vector<float> > output;
    //auto start = chrono::high_resolution_clock::now();
    if (option==1)
    {
        output = matrix_multiply(h_2d, resized_input);
        //cout<<"normal"<<endl;
    }
    else if (option==2)
    {
        output = matrix_multiply_with_mkl(h_2d, resized_input);
        //cout<<"mkl"<<endl;
    }
    else if (option==3)
    {
        output = matrix_multiply_with_openblas(h_2d, resized_input);
        //cout<<"openblas"<<endl;
    }
    else if(option==4)
    {
        //cout<<"starting matrix multiply"<<endl;
        output = matrix_multiply_with_pthreads(h_2d, resized_input);
        //vector<vector<float> > mat3 = matrix_multiply_with_pthreads(h_2d, resized_input);
        //cout<<"pthreads"<<endl;
    }

    //auto finish = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = finish - start;
    //double exec_time = elapsed.count();

    vector<vector<float> > resized_output(m+k-1,vector<float>(m+k-1));

    p=0;
    for(int i=0;i<m+k-1;i++)
    {
        for(int j=0;j<m+k-1;j++)
        {
            resized_output[i][j]=output[p][0];
            p++;
        }
    }
    vector<vector<float> > trimmed_output(m-k+1, vector<float>(m-k+1,0));
    for(int i=k-1;i<m;i++)
    {
        for(int j=k-1;j<m;j++)
        {
            trimmed_output[i-k+1][j-k+1]=resized_output[i][j];
        }
    }
    // cout<<"Execution time for matrix multiplication in seconds= "<<elapsed.count()<<endl<<endl;
     return trimmed_output;
    // multiple_return_variables ret;
    // ret.matrix=trimmed_output;
    //ret.elapsed=elapsed;
    // return ret;
}

vector<vector<float> > read_matrix(string s, int rows)
{
    try {
        ifstream infile;
        infile.open (s);
        string str;
        if (!infile)
        {
            throw "The file can't be opened, kindly check the path";
        }
        vector<vector<float> > mat;
        for (int i=0; i<rows; i++)
        {
            vector<float> v(rows,0);
            mat.push_back(v);
        }
        float data;
        for (int i=0; i<rows;i++)
        {
            for (int j=0; j<rows; j++)
            {
                getline(infile, str);
                data=stof(str);
                mat[j][i]=data;
            }
        }
        return mat;
    } catch (std::invalid_argument) {
        throw "The data in the file provided by you is not valid";
    } catch (const char* msg)
    {
        throw msg;
    }

}

int main(int argc, char *argv[])
{
    try {
        if (argc==3)
        {
            if (strcmp(argv[1], "sigmoid")==0)
            {
                vector<float> input=read_file(argv[2]);
                vector<float> ans=sigmoid(input);
                int n=static_cast<int>(ans.size());
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<n; i++)
                {
                    cout<<ans[i]<<endl;
                    outfile<<ans[i]<<endl;
                }
                //cout<<endl;
                outfile.close();
            }
            else if (strcmp(argv[1], "softmax")==0)
            {
                vector<float> input=read_file(argv[2]);
                vector<float> ans=softmax(input);
                int n=static_cast<int>(ans.size());
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<n; i++)
                {
                    cout<<ans[i]<<endl;
                    outfile<<ans[i]<<endl;
                }
                //cout<<endl;
                outfile.close();
            }
            else
            {
                throw "Your function call is invalid, maybe you wanted to call softmax or sigmoid ?";
            }
        }
        else if (argc==4)
        {
            if (strcmp(argv[1], "relu")==0)
            {
                int rows=stoi(argv[3]);
                if (rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                vector<vector<float> > input=read_matrix(argv[2], rows);
                //cout<<argv[2]<<endl;
                vector<vector<float> > ans=relu(input);
                for(int i=0; i<rows; i++)
                {
                    for(int j=0;j<rows;j++)
                    {
                        cout<<ans[i][j]<<" ";
                    }
                    cout<<endl;
                }
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<rows; i++)
                {
                    for(int j=0;j<rows;j++)
                    {
                        outfile<<ans[j][i]<<endl;
                    }
                    //cout<<endl;
                }
                outfile.close();
            }
            else if(strcmp(argv[1], "tanh")==0)
            {
                int rows=stoi(argv[3]);
                if (rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                vector<vector<float> > input=read_matrix(argv[2], rows);
                vector<vector<float> > ans=tanh(input);
                for(int i=0; i<rows; i++)
                {
                    for(int j=0;j<rows;j++)
                    {
                        cout<<ans[i][j]<<" ";
                    }
                    cout<<endl;
                }
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<rows; i++)
                {
                    for(int j=0;j<rows;j++)
                    {
                        outfile<<ans[j][i]<<endl;
                    }
                    //cout<<endl;
                }
                outfile.close();
            }
            else
            {
                throw "Your function call is invalid, maybe you wanted to call relu or tanh ?";
            }
        }
        else if (argc==6)
        {
            if (strcmp(argv[1], "maxpool")==0)
            {
                int rows=stoi(argv[3]);
                int width=stoi(argv[4]);
                int stride=stoi(argv[5]);
                if (rows<0 || width<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (stride<=0)
                {
                    throw "The stride has to be a positive number";
                }
                vector<vector<float> > input=read_matrix(argv[2], rows);
                vector<vector<float> > ans=maxpooling(input, width, stride);
                int ans_rows=static_cast<int>(ans.size());
                for(int i=0; i<ans_rows; i++)
                {
                    for(int j=0;j<ans_rows;j++)
                    {
                        cout<<ans[i][j]<<" ";
                    }
                    cout<<endl;
                }
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<ans_rows; i++)
                {
                    for(int j=0;j<ans_rows;j++)
                    {
                        outfile<<ans[j][i]<<endl;
                    }
                    //cout<<endl;
                }
                outfile.close();
            }
            else if (strcmp(argv[1], "average_pool")==0)
            {
                int rows=stoi(argv[3]);
                int width=stoi(argv[4]);
                int stride=stoi(argv[5]);
                if (rows<0 || width<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (stride<=0)
                {
                    throw "The stride has to be a positive number";
                }
                vector<vector<float> > input= read_matrix(argv[2], rows);
                vector<vector<float> > ans= average_pooling(input, width, stride);
                int ans_rows=static_cast<int>(ans.size());
                for(int i=0; i<ans_rows; i++)
                {
                    for(int j=0;j<ans_rows;j++)
                    {
                        cout<<ans[i][j]<<" ";
                    }
                    cout<<endl;
                }
                ofstream outfile;
                outfile.open("output.txt");
                for(int i=0; i<ans_rows; i++)
                {
                    for(int j=0;j<ans_rows;j++)
                    {
                        outfile<<ans[j][i]<<endl;
                    }
                    //cout<<endl;
                }
                outfile.close();
            }
            else if (strcmp(argv[1], "convolve_without_padding")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    vector<vector<float> > input= read_matrix(argv[2], m_rows);
                    vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                    vector<vector<float> > ans=convolve(input, kernel);
                    int ans_rows=static_cast<int>(ans.size());
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            cout<<ans[i][j]<<" ";
                        }
                        cout<<endl;
                    }
                    ofstream outfile;
                    outfile.open("output.txt");
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            outfile<<ans[j][i]<<endl;
                        }
                        //cout<<endl;
                    }
                    outfile.close();
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else if (strcmp(argv[1], "convolve_with_padding")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    if (k_rows%2==1)
                    {
                        vector<vector<float> > input=read_matrix(argv[2], m_rows);
                        vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                        int pad=(k_rows-1)/2;
                        vector<vector<float> > padded_input=padding(input, pad);
                        vector<vector<float> > ans=convolve(padded_input, kernel);
                        int ans_rows=static_cast<int>(ans.size());
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                cout<<ans[i][j]<<" ";
                            }
                            cout<<endl;
                        }
                        ofstream outfile;
                        outfile.open("output.txt");
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                outfile<<ans[j][i]<<endl;
                            }
                            //cout<<endl;
                        }
                        outfile.close();
                    }
                    else
                    {
                        throw "If kernel contains odd rows, padding is not possible.";
                    }
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else if (strcmp(argv[1], "convolve_with_padding_matrixmult")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    if (k_rows%2==1)
                    {
                        vector<vector<float> > input=read_matrix(argv[2], m_rows);
                        vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                        int pad=(k_rows-1)/2;
                        vector<vector<float> > padded_input=padding(input, pad);

                        clock_t start = clock();
                        vector<vector<float> > ans=toeplitz_convolve(padded_input, kernel, 1);
                        clock_t stop = clock();


                        int ans_rows=static_cast<int>(ans.size());
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                cout<<ans[i][j]<<" ";
                            }
                            cout<<endl;
                        }
                        cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                        ofstream outfile;
                        outfile.open("output.txt");
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                outfile<<ans[j][i]<<endl;
                            }
                            //cout<<endl;
                        }
                        outfile.close();
                    }
                    else
                    {
                        throw "If kernel contains odd rows, padding is not possible.";
                    }
                }
                else
                {
                    throw "The kernel matrix should be smalller than input matrix.";
                }

            }
            else if (strcmp(argv[1], "convolve_with_padding_matrixmult_with_mkl")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    if (k_rows%2==1)
                    {
                        vector<vector<float> > input=read_matrix(argv[2], m_rows);
                        vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                        int pad=(k_rows-1)/2;
                        vector<vector<float> > padded_input=padding(input, pad);

                        clock_t start = clock();
                        vector<vector<float> > ans=toeplitz_convolve(padded_input, kernel, 2);
                        clock_t stop = clock();

                        int ans_rows=static_cast<int>(ans.size());
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                cout<<ans[i][j]<<" ";
                            }
                            cout<<endl;
                        }
                        cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;

                        ofstream outfile;
                        outfile.open("output.txt");
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                outfile<<ans[j][i]<<endl;
                            }
                            //cout<<endl;
                        }
                        outfile.close();
                    }
                    else
                    {
                        throw "If kernel contains odd rows, padding is not possible.";
                    }
                }
                else
                {
                    throw "The kernel matrix should be smalller than input matrix.";
                }

            }
            else if (strcmp(argv[1], "convolve_with_padding_matrixmult_with_openblas")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    if (k_rows%2==1)
                    {
                        vector<vector<float> > input=read_matrix(argv[2], m_rows);
                        vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                        int pad=(k_rows-1)/2;
                        vector<vector<float> > padded_input=padding(input, pad);

                        clock_t start = clock();
                        vector<vector<float> > ans=toeplitz_convolve(padded_input, kernel, 3);
                        clock_t stop = clock();

                        int ans_rows=static_cast<int>(ans.size());
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                cout<<ans[i][j]<<" ";
                            }
                            cout<<endl;
                        }
                        cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                        ofstream outfile;
                        outfile.open("output.txt");
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                outfile<<ans[j][i]<<endl;
                            }
                            //cout<<endl;
                        }
                        outfile.close();
                    }
                    else
                    {
                        throw "If kernel contains odd rows, padding is not possible.";
                    }
                }
                else
                {
                    throw "The kernel matrix should be smalller than input matrix.";
                }

            }
            else if (strcmp(argv[1], "convolve_with_padding_matrixmult_with_pthreads")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    if (k_rows%2==1)
                    {
                        vector<vector<float> > input=read_matrix(argv[2], m_rows);
                        vector<vector<float> > kernel=read_matrix(argv[4], k_rows);
                        int pad=(k_rows-1)/2;
                        vector<vector<float> > padded_input=padding(input, pad);

                        clock_t start = clock();
                        vector<vector<float> > ans=toeplitz_convolve(padded_input, kernel, 4);
                        clock_t stop = clock();

                        int ans_rows=static_cast<int>(ans.size());
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                cout<<ans[i][j]<<" ";
                            }
                            cout<<endl;
                        }
                        cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                        ofstream outfile;
                        outfile.open("output.txt");
                        for(int i=0; i<ans_rows; i++)
                        {
                            for(int j=0;j<ans_rows;j++)
                            {
                                outfile<<ans[j][i]<<endl;
                            }
                            //cout<<endl;
                        }
                        outfile.close();
                    }
                    else
                    {
                        throw "If kernel contains even rows, padding is not possible.";
                    }
                }
                else
                {
                    throw "The kernel matrix should be smalller than input matrix.";
                }

            }
            else if (strcmp(argv[1], "convolve_without_padding_matrixmult")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    vector<vector<float> > input= read_matrix(argv[2], m_rows);
                    vector<vector<float> > kernel=read_matrix(argv[4], k_rows);

                    clock_t start = clock();
                    vector<vector<float> > ans=toeplitz_convolve(input, kernel, 1);
                    clock_t stop = clock();

                    int ans_rows=static_cast<int>(ans.size());
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            cout<<ans[i][j]<<" ";
                        }
                        cout<<endl;
                    }
                    cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                    ofstream outfile;
                    outfile.open("output.txt");
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            outfile<<ans[j][i]<<endl;
                        }
                        //cout<<endl;
                    }
                    outfile.close();
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else if (strcmp(argv[1], "convolve_without_padding_matrixmult_with_mkl")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    vector<vector<float> > input= read_matrix(argv[2], m_rows);
                    vector<vector<float> > kernel=read_matrix(argv[4], k_rows);

                    clock_t start = clock();
                    vector<vector<float> > ans=toeplitz_convolve(input, kernel, 2);
                    clock_t stop = clock();

                    int ans_rows=static_cast<int>(ans.size());
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            cout<<ans[i][j]<<" ";
                        }
                        cout<<endl;
                    }
                    cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                    ofstream outfile;
                    outfile.open("output.txt");
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            outfile<<ans[j][i]<<endl;
                        }
                        //cout<<endl;
                    }
                    outfile.close();
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else if (strcmp(argv[1], "convolve_without_padding_matrixmult_with_openblas")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    vector<vector<float> > input= read_matrix(argv[2], m_rows);
                    vector<vector<float> > kernel=read_matrix(argv[4], k_rows);

                    clock_t start = clock();
                    vector<vector<float> > ans=toeplitz_convolve(input, kernel, 3);
                    clock_t stop = clock();

                    int ans_rows=static_cast<int>(ans.size());
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            cout<<ans[i][j]<<" ";
                        }
                        cout<<endl;
                    }
                    cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                    ofstream outfile;
                    outfile.open("output.txt");
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            outfile<<ans[j][i]<<endl;
                        }
                        //cout<<endl;
                    }
                    outfile.close();
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else if (strcmp(argv[1], "convolve_without_padding_matrixmult_with_pthreads")==0)
            {
                int m_rows=static_cast<int>(stoi(argv[3]));
                int k_rows=static_cast<int>(stoi(argv[5]));
                if (m_rows<0 || k_rows<0)
                {
                    throw "The number of rows should be non-negative";
                }
                if (m_rows>=k_rows)
                {
                    vector<vector<float> > input= read_matrix(argv[2], m_rows);
                    vector<vector<float> > kernel=read_matrix(argv[4], k_rows);

                    clock_t start = clock();
                    vector<vector<float> > ans=toeplitz_convolve(input, kernel, 4);
                    clock_t stop = clock();

                    int ans_rows=static_cast<int>(ans.size());
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            cout<<ans[i][j]<<" ";
                        }
                        cout<<endl;
                    }
                    cout<<endl<<"Running time for convolution = "<<(double)(stop-start)/(double)(CLOCKS_PER_SEC)<<endl;
                    ofstream outfile;
                    outfile.open("output.txt");
                    for(int i=0; i<ans_rows; i++)
                    {
                        for(int j=0;j<ans_rows;j++)
                        {
                            outfile<<ans[j][i]<<endl;
                        }
                        //cout<<endl;
                    }
                    outfile.close();
                }
                else
                {
                    throw "The kernel matrix should be smaller than input matrix.";
                }
            }
            else
            {
                throw "Your function call is invalid, maybe you wanted to call subsampling or convolution related functions";
            }
        }
        else
        {
            throw "The number of arguments you have given is inappropriate";
        }
    } catch (std::invalid_argument) {
        cout<<"You have given some invalid arguments"<<endl;
        cout<<"Please refer to README for further help"<<endl;
    } catch (const char* msg)
    {
        cout<<msg<<endl;
        cout<<"Please refer to README for further help"<<endl;
    }
    catch (...)
    {
        cout<<"There is something wrong with the arguments you have provided or the files you are using to input matrices/vectors"<<endl;
        cout<<"Please refer to README for further help"<<endl;
    }

    /////////////////////////////////////////////////////
    // Randomised portion of code
    srand(time(NULL));
    ofstream exec_file;
    exec_file.open("exec.txt");
    for(int i=2; i<500; i++)
    {
        vector<vector<float> > input(50, vector<float>(50,0));
        vector<vector<float> > kernel(50, vector<float>(50,0));

        for(int x=0;x<50;x++)
        {
            for(int y=0;y<50;y++)
            {
                input[x][y]=rand();
            }
        }

        for(int x=0;x<50;x++)
        {
            for(int y=0;y<50;y++)
            {
                kernel[x][y]=rand();
            }
        }


        clock_t start1 = clock();
        vector<vector<float> > ans=matrix_multiply(input, kernel);
        clock_t stop1 = clock();

        clock_t start2 = clock();
        ans=matrix_multiply_with_mkl(input, kernel);
        clock_t stop2 = clock();

        clock_t start3 = clock();
        ans=matrix_multiply_with_openblas(input, kernel);
        clock_t stop3 = clock();

        clock_t start4 = clock();
        ans=matrix_multiply_with_pthreads(input, kernel);
        clock_t stop4 = clock();

        exec_file<<(double)(stop1-start1)/(double)(CLOCKS_PER_SEC)<<" ";
        exec_file<<(double)(stop2-start2)/(double)(CLOCKS_PER_SEC)<<" ";
        exec_file<<(double)(stop3-start3)/(double)(CLOCKS_PER_SEC)<<" ";
        exec_file<<(double)(stop4-start4)/(double)(CLOCKS_PER_SEC)<<" ";
        exec_file<<endl;
        cout<<i<<endl;
    }
    return 0;
}
