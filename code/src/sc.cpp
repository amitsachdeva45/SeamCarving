
#include "sc.h"

using namespace cv;
using namespace std;
#include<time.h>
#include<iostream>
/*
*http://www.cs.princeton.edu/courses/archive/spring14/cos226/assignments/seamCarving.html
*/



bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}
/*
* Assignment and Calculation of Energy using Dynamic Approach
*/
vector<int> find_path_vertical(Mat &in_image, vector<vector<int> > intensity, vector<vector<int> > finalEnergy)
{
    int rows =  in_image.rows;
    int cols = in_image.cols;
    int min_value = numeric_limits<int>::max();
    int min_index = -1;
    for (int i = 0; i < cols; i++) {
        if(finalEnergy[rows-1][i] < min_value) {
            min_value = finalEnergy[rows-1][i];
            min_index = i;
        }
    }
    vector<int> path(rows);
    int x_axis = rows - 1;
    int y_axis = min_index;
    path[x_axis] = y_axis;

    while(x_axis != 0) {
        int row = x_axis, col = y_axis;
        int value = finalEnergy[row][col] - intensity[row][col];
        if(col == 0) {
            x_axis = row-1;
            if(value == finalEnergy[row-1][col]) {
                y_axis = col;
            } else {                
                y_axis = col+1;
            }
        } else if(col == cols - 1) {
            x_axis = row-1;
            if(value == finalEnergy[row-1][col]) {                
                y_axis = col;
            } else {
                y_axis = col-1;
            }
        } else {
            x_axis = row-1;
            if(value == finalEnergy[row-1][col-1]) {                
                y_axis = col-1;
            } else if(value == finalEnergy[row-1][col]) {
                y_axis = col;
            } else {
                y_axis = col+1;
            }
        }
        path[x_axis] = y_axis;
    }
    return path;
}
vector<int> find_path_horizontal(Mat &in_image, vector<vector<int> > intensity, vector<vector<int> > finalEnergy)
{
    int rows =  in_image.rows;
    int cols = in_image.cols;
    int min_value = numeric_limits<int>::max();
    int min_index = -1;
    for (int i = 0; i < rows; i++) {
        if(finalEnergy[i][cols-1] < min_value) {
            min_value = finalEnergy[i][cols-1];
            min_index = i;
        }
    }
    vector<int> path(cols);
    int y_axis = cols - 1;
    int x_axis = min_index;
    path[y_axis] = x_axis;

    while(y_axis != 0) {
        int row = x_axis, col = y_axis;
        int value = finalEnergy[row][col] - intensity[row][col];
        if(row == 0) {
            y_axis = col-1;
            if(value == finalEnergy[row][col-1]) {
                x_axis = row;
            } else {                
                x_axis = row+1;
            }
        } else if(row == rows - 1) {
            y_axis = col-1;
            if(value == finalEnergy[row][col-1]) {                
                x_axis = row;
            } else {
                x_axis = row-1;
            }
        } else {
            y_axis = col-1;
            if(value == finalEnergy[row-1][col-1]) {                
                x_axis = row-1;
            } else if(value == finalEnergy[row][col-1]) {
                x_axis = row;
            } else {
                x_axis = row+1;
            }
        }
        path[y_axis] = x_axis;
    }
    return path;
}
double calculate_particular_block_intensity(Mat &in_image, int i, int j) {

    Vec3b left = in_image.at<Vec3b>((i != 0) ? i - 1 : in_image.rows - 1, j);
    Vec3b right = in_image.at<Vec3b>((i != in_image.rows - 1) ? i + 1 : 0, j);
    Vec3b top = in_image.at<Vec3b>(i, (j != 0) ? j - 1 : in_image.cols - 1);
    Vec3b bottom = in_image.at<Vec3b>(i, (j != in_image.cols - 1) ? j + 1 : 0);
    double pixelX = pow(right[0] - left[0], 2) + pow(right[1] - left[1], 2) + pow(right[2] - left[2], 2);
    double pixelY = pow(bottom[0] - top[0], 2) + pow(bottom[1] - top[1], 2) + pow(bottom[2] - top[2], 2);

    return sqrt(pixelX + pixelY);
}
vector<int> intensity_assignment(Mat &in_image, Mat &out_image,int index) {
    int rows = in_image.rows;
    int cols = in_image.cols;
    vector<vector<int> > intensity(rows);
    vector<vector<int> > finalEnergy(rows);
    if(index == 1){
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                
                intensity[i].push_back((int) calculate_particular_block_intensity(in_image, i, j));
                if(i==0)
                {
                    finalEnergy[0].push_back(intensity[0][j]);
                }
                else
                {
                    if(j == 0)
                        finalEnergy[i].push_back(intensity[i][j] + min(finalEnergy[i-1][j], finalEnergy[i-1][j+1]));
                    else if(j == cols-1)
                        finalEnergy[i].push_back(intensity[i][j] + min(finalEnergy[i-1][j-1], finalEnergy[i-1][j]));
                    else
                        finalEnergy[i].push_back(intensity[i][j] + min(min(finalEnergy[i-1][j], finalEnergy[i-1][j+1]), finalEnergy[i-1][j-1])); 
                }
                
            }
        }
    
        return find_path_vertical(in_image, intensity, finalEnergy);
    }
    else
    {
        for (int j = 0; j < cols; j++) {
            for (int i = 0; i < rows; i++) {           
                
                intensity[i].push_back((int) calculate_particular_block_intensity(in_image, i, j));
                if(j==0)
                {
                    finalEnergy[i].push_back(intensity[i][0]);
                }
                else
                {
                    if(i == 0)
                        finalEnergy[i].push_back(intensity[i][j] + min(finalEnergy[i][j-1], finalEnergy[i+1][j-1]));
                    else if(i == rows-1)
                        finalEnergy[i].push_back(intensity[i][j] + min(finalEnergy[i-1][j-1], finalEnergy[i][j-1]));
                    else
                        finalEnergy[i].push_back(intensity[i][j] + min(min(finalEnergy[i-1][j-1], finalEnergy[i+1][j-1]), finalEnergy[i][j-1])); 
                }
                
            }
        }
        return find_path_horizontal(in_image, intensity, finalEnergy);
    }
    
    
}

// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    
    cout<<"Vertical"<<endl;
    while (iimage.cols > new_width) {
        vector<int> path = intensity_assignment(iimage,oimage,1); 
        reduce_vertical_seam_trivial(iimage, path);
        oimage = iimage.clone();
        cout<<"Y-AXIS= "<<iimage.rows<<","<<"X-AXIS= "<<iimage.cols<<endl;
    }

    cout<<"Horizontal"<<endl;
    while (iimage.rows > new_height) {
        vector<int> path = intensity_assignment(iimage,oimage,0);
        reduce_horizontal_seam_trivial(iimage, path);
        oimage = iimage.clone();
        cout<<"Y-AXIS= "<<iimage.rows<<","<<"X-AXIS= "<<iimage.cols<<endl;
    }
    out_image = oimage.clone();
    return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, vector<int> path){

    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols-1;
    
    // create an image slighly smaller
    Mat new_image = Mat(rows, cols, CV_8UC3);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if(j >= path[i])
                new_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j+1);
            else
                new_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
        }
    }

    new_image.copyTo(in_image);

    return true;
}


// vertical trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, vector<int> path){
    // retrieve the dimensions of the new image
    int rows = in_image.rows-1;
    int cols = in_image.cols;
    
    // create an image slighly smaller
    Mat new_image = Mat(rows, cols, CV_8UC3);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if(i >= path[j])
                new_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i+1, j);
            else
                new_image.at<Vec3b>(i, j) = in_image.at<Vec3b>(i, j);
        }
    }

    new_image.copyTo(in_image);

    return true;
}

