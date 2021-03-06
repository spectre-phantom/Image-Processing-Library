#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string.h>
//#include "functions.h"
#include "function_convolution.h"
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
    vector<vector<float> > reflected_kernel = kernel;
    //WARNING: This was done in order to make our answers match for LeNet CNN.
    //vector<vector<float> > reflected_kernel = reflect_kernel(kernel);
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

// vector<vector<float>> toeplitz_convolve(vector<vector<float> > matrix, vector<vector<float> > kernel, int option)
// {
//     if(matrix.size()>1)
//     {
//         int m = static_cast<int>(matrix.size());
//         int k = static_cast<int>(kernel.size());
//         int dim = m+k-1;
//         vector<vector<float> > padded_kernel(dim, vector<float>(dim,0));
//
//         for(int i = 0; i<k; i++)
//         {
//             for(int j=0; j<k; j++)
//             {
//                 padded_kernel[i][j]=kernel[i][j];
//             }
//         }
//
//         vector<vector<vector<float> > > h(dim,vector<vector<float> >(dim,vector<float>(m,0)));
//
//         for(int z=0; z<dim; z++)
//         {
//             vector<float> a = padded_kernel[z];
//             int i=0;
//             for(int y=0; y<m; y++)
//             {
//                 for (int x = i; x < dim; x++)
//                 {
//                     h[z][x][y]=a[x-i];
//                 }
//                 i++;
//             }
//         }
//
//         vector<vector<float> > h_2d(dim*dim, vector<float>(m*m,0));
//         int b=0;//Poorva-I don't know if it is to be made float or not
//         for(int i=0; i<dim*dim; i+=dim)
//         {
//             b=i/dim;
//             for(int j=0;j<m*m; j+=m)
//             {
//                 if(b<0)
//                 {
//                     b=dim-1;
//                 }
//                 for(int p=i;p<i+dim;p++)
//                 {
//                     for(int l=j;l<j+m;l++)
//                     {
//                         h_2d[p][l]=h[b][p-i][l-j];
//                     }
//                 }
//                 b--;
//             }
//         }
//
//         vector<vector<float> > resized_input(m*m,vector<float>(1,0));
//         int p=0;
//         for(int i=0; i<m; i++)
//         {
//             for(int j=0; j<m; j++)
//             {
//                 resized_input[p][0]=matrix[i][j];
//                 p++;
//             }
//         }
//         vector<vector<float> > output;
//         //auto start = chrono::high_resolution_clock::now();
//         if (option==1)
//         {
//             output = matrix_multiply(h_2d, resized_input);
//             //cout<<"normal"<<endl;
//         }
//         else if (option==2)
//         {
//             output = matrix_multiply_with_mkl(h_2d, resized_input);
//             //cout<<"mkl"<<endl;
//         }
//         else if (option==3)
//         {
//             output = matrix_multiply_with_openblas(h_2d, resized_input);
//             //cout<<"openblas"<<endl;
//         }
//         else if(option==4)
//         {
//             //cout<<"starting matrix multiply"<<endl;
//             output = matrix_multiply_with_pthreads(h_2d, resized_input);
//             //vector<vector<float> > mat3 = matrix_multiply_with_pthreads(h_2d, resized_input);
//             //cout<<"pthreads"<<endl;
//         }
//
//         //auto finish = std::chrono::high_resolution_clock::now();
//         //std::chrono::duration<double> elapsed = finish - start;
//         //double exec_time = elapsed.count();
//
//         vector<vector<float> > resized_output(m+k-1,vector<float>(m+k-1));
//
//         p=0;
//         for(int i=0;i<m+k-1;i++)
//         {
//             for(int j=0;j<m+k-1;j++)
//             {
//                 resized_output[i][j]=output[p][0];
//                 p++;
//             }
//         }
//         vector<vector<float> > trimmed_output(m-k+1, vector<float>(m-k+1,0));
//         for(int i=k-1;i<m;i++)
//         {
//             for(int j=k-1;j<m;j++)
//             {
//                 trimmed_output[i-k+1][j-k+1]=resized_output[i][j];
//             }
//         }
//         // cout<<"Execution time for matrix multiplication in seconds= "<<elapsed.count()<<endl<<endl;
//          return trimmed_output;
//         // multiple_return_variables ret;
//         // ret.matrix=trimmed_output;
//         //ret.elapsed=elapsed;
//         // return ret;
//     }
//     else if(matrix.size()==1)
//     {
//         vector<vector<float>> output (1, vector<float>(1,0));
//         output[0][0]=matrix[0][0]*kernel[0][0];
//         return output;
//     }
// }
//
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
    try
    {
        if (argc==1)
        {
            vector<float> out = lenet("data.txt","conv1.txt","conv2.txt","fc1.txt","fc2.txt");
            vector<tuple<float,int>> ans;
            for(int i=0; i<out.size(); i++)
            {
                ans.push_back(make_tuple(out[i],i));
            }
            sort(ans.begin(), ans.end());
            ofstream outfile;
            outfile.open("output.txt");
            outfile<<"Digit"<<"\t"<<"Probability"<<endl;
            cout<<"Digit"<<"\t"<<"Probability"<<endl;
            for(int i=9; i>4; i--)
            {
                outfile<<(get<1>(ans[i]))<<"\t"<<(get<0>(ans[i]))<<endl;
                cout<<(get<1>(ans[i]))<<"\t"<<(get<0>(ans[i]))<<endl;
            }
            outfile.close();
        }
        else{
          cout<<"There is something wrong with the arguments you have provided or the files you are using to input matrices/vectors"<<endl;
          cout<<"Please refer to README for further help"<<endl;
        }
    }
    catch (...)
    {
        cout<<"There is something wrong with the arguments you have provided or the files you are using to input matrices/vectors"<<endl;
        cout<<"Please refer to README for further help"<<endl;
    }
    return 0;
}
