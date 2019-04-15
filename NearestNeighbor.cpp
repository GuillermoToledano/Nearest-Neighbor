/* 
 * File:   main.cpp
 * Author: GuillermoToledano
 *
 * Created on February 26, 2019, 12:16 PM
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>
#include <istream>
#include <sstream>
#include <string>
#define FILE "Data Files/Features.csv"
#define RFILE "Data Files/NN-Results.txt"
#define TFFILE "Data Files/TF-Results.txt"
#define FOLDS 10

using namespace std;

/** 
 * Object Class
 *  
 * oBall1 = 1
 * oBall2 = 2
 * oCD1   = 3
 * oCD2   = 4
 * oCD3   = 5
 * oCD4   = 6
 */
enum oClass {
    oBall1 = 1,
    oBall2 = 2,
    oCD1 = 3,
    oCD2 = 4,
    oCD3 = 5,
    oBall6 = 6
};

/** 
 * Object Features 
 * @Radio Estimated object's radio.
 * @Ratio Width-Height relation
 * @White Object's white percentage
 * @Black Object's black percentage
 * @Green Object's green percentage
 * @BwotG Object's black without green percentage
 */
struct oFeatures {
    double Radio;
    double Ratio;
    double White;
    double Black;
    double BoutG;
    double Green;
};

/** 
 * Object Sample 
 * @Features From oFeatures struct
 * @Class From oClass struct
 */
struct oSample {
    oFeatures Features;
    oClass Class;
};

struct oRSample {
    oSample Sample;
    int Index;
};

/** 
 * Object Sample K
 * @Sample From oSample struct
 * @K Assigned K value 
 */
struct oKSample {
    oSample Sample;
    int K;
};

struct oDist {
    oClass Class;
    int Distance;
};

struct oData {
    int nSamples;
    int nFeatures;
    int nClasses;
    int nRandom;
    int nSPI;
} Features;

// Prototypes - Functions with two-dimensional arrays
oData read_features();
oData count_samples();
void readCSV(float **mSamples, float **mClasses, oData Features);
void read_data(float **mSamples, float **mClasses, oData Features);
void count_instances(float **mSamples, float **mClasses, oData Features);
void show_data(float **mSamples, int ROWS, int COLS);
void show_instances(float **mClasses, int ROWS, int COLS);
void select_random(float **mSamples, float **mClasses, float **mRandom, oData Features);
void show_random(float **mRandom, int ROWS, int COLS);
void nearest_neighbor(float **mSamples, float **mRandom, oData Features);
void toArray(float **mSamples, float *vSample, int Row, int Col);
float Euclidean(float *rVector, float *sVector, int Size);
void sort_data(float **vDistances, int Rows);
void show_distances(float **vDistances, int Rows);
int select_K(float **mDistances, float sClass, int gSize);
int getOdd(int N);
int getCount(int Min, int Max);
float Manhattan(float *cVector, float *sVector, int Size);
void write_data(float **Data, oData Features, int Mode);
void write_results(float *Data, int Cols, int Mode);
void getFold(float **Dataset, float *Fold, int ROW, int COLS);

int main(int argc, char** argv) {
    Features = read_features();
    int FEATURES = Features.nFeatures;
    int CLASSES = Features.nClasses;
    int SIZE = Features.nSamples;
    int RANDOM = SIZE * 0.10;
    Features.nSPI = 3;
    int target = CLASSES * Features.nSPI;
    while (RANDOM != target) {
        if (RANDOM < target) {
            RANDOM++;
        } else {
            RANDOM--;
        }
    }
    Features.nRandom = RANDOM;

    cout << "Total Samples:\t" << SIZE << endl;
    cout << "Total Features:\t" << FEATURES << endl;
    cout << "Total Classes:\t" << CLASSES << endl;
    cout << "Random Samples:\t" << RANDOM << endl;
    //Arrays
    float** Dataset = new float*[SIZE];
    for (int i = 0; i < SIZE; i++) {
        Dataset[i] = new float[FEATURES];
    }
    float** RSelected = new float*[RANDOM];
    for (int i = 0; i < RANDOM; i++) {
        RSelected[i] = new float[FEATURES + 3];
    }
    float** Instances = new float*[CLASSES];
    for (int i = 0; i < CLASSES; i++) {
        Instances[i] = new float[2];
    }
    // Two-dimensional Array Operations
    readCSV(Dataset, Instances, Features);
    select_random(Dataset, Instances, RSelected, Features);
    nearest_neighbor(Dataset, RSelected, Features);
    show_random(RSelected, RANDOM, FEATURES + 3);

    //Ten Fold Cross Validation
    int fold_size = SIZE / 10;
    cout << "Fold Size: " << fold_size << endl;
    float** Fold = new float*[fold_size];
    for (int i = 0; i < fold_size; i++ ) {
        Fold[i] = new float[FEATURES + 1];
    }
    int fold_cont = 0, current_row = 0;
    while (fold_cont < FOLDS) {
        
    }
    return 0;
}

oData read_features() {
    const string attribute = "@attribute";
    const string oclass = "class";
    const string data = "@data";
    const char oCBracket = '{';
    const char cCBracket = '}';
    const char comment = '%';
    oData Features;
    ifstream file;
    file.open(FILE, ios::in);
    if (file.is_open()) {
        string sWord;
        string sClass;
        string sBracket;
        int nSamples = 0;
        int nFeatures = 0;
        int nInstances = 0;
        bool reading_attrs = true;
        while (reading_attrs) {
            file >> sWord;
            if (sWord[0] == comment) {
                continue;
            }
            if (sWord.compare(attribute) == 0) {
                nFeatures++;
                file >> sClass;
                if (sClass.compare(oclass) == 0) {
                    //cout << sClass << endl;
                    file >> sBracket;
                    if (sBracket[0] == oCBracket) {
                        //cout << sBracket << endl;
                        file >> sBracket;
                        while (sBracket[0] != cCBracket) {
                            //cout << sBracket << endl;
                            nInstances++;
                            file >> sBracket;
                        }
                    }
                }
            }
            if (sWord.compare(data) == 0) {
                reading_attrs = false;
                while (!file.eof()) {
                    getline(file, sWord);
                    if (!sWord.empty() && sWord.length() > 1) {
                        nSamples++;
                    }
                }
            }
        }
        Features.nFeatures = nFeatures;
        Features.nSamples = nSamples;
        Features.nClasses = nInstances;
        file.close();
    }
    return Features;
}

void readCSV(float **mSamples, float **mClasses, oData Features) {
    int INST = Features.nClasses;
    int COLS = Features.nFeatures;
    int ROWS = Features.nSamples;
    const string attribute = "@attribute";
    const string oclass = "class";
    const string oCBracket = "{";
    const string data = "@data";
    bool search_data = true;
    ifstream file;
    file.open(FILE, ios::in);
    if (file.is_open()) {
        cout << "Reading data..." << endl;
        string sWord;
        while (search_data) {
            file >> sWord;
            if (sWord.compare(attribute) == 0) {
                file >> sWord;
                if (sWord.compare(oclass) == 0) {
                    file >> sWord;
                    if (sWord.compare(oCBracket) == 0) {
                        float instance;
                        for (int i = 0; i < INST; i++) {
                            file >> instance;
                            mClasses[i][0] = instance;
                        }
                    }
                }
            }
            if (sWord.compare(data) == 0) {
                search_data = false;
                float feature;
                string line;
                int row = 0;
                while (getline(file, line)) {
                    if (line.empty() || line.length() < 2) {
                        continue;
                    }
                    stringstream sLine(line);
                    string word;
                    int col = 0;
                    while (getline(sLine, word, ',')) {
                        feature = stof(word);
                        float rest = feature - ((int) feature);
                        if (rest > 0) {
                            feature = round(feature * 100.0 + 0.5) / 100.0;
                        }
                        mSamples[row][col] = feature;
                        col++;
                    }
                    row++;
                }
            }
        }
        file.close();
        count_instances(mSamples, mClasses, Features);
        //show_instances(mClasses, INST, 2);
        show_data(mSamples, ROWS, COLS);
        //write_data(mSamples, Features, 3);
    }
}

void read_data(float **mSamples, float **mClasses, oData Features) {
    int INST = Features.nClasses;
    int COLS = Features.nFeatures;
    int ROWS = Features.nSamples;
    const string attribute = "@attribute";
    const string oclass = "class";
    const string oCBracket = "{";
    const string data = "@data";
    bool search_data = true;
    ifstream file;
    file.open(FILE, ios::in);
    if (file.is_open()) {
        string sWord;
        cout << "Reading data..." << endl;
        while (search_data) {
            file >> sWord;
            if (sWord.compare(attribute) == 0) {
                //cout << sWord << endl;
                file >> sWord;
                if (sWord.compare(oclass) == 0) {
                    //cout << sWord << endl;
                    file >> sWord;
                    if (sWord.compare(oCBracket) == 0) {
                        //cout << sWord << endl;
                        float instance;
                        for (int i = 0; i < INST; i++) {
                            file >> instance;
                            mClasses[i][0] = instance;
                        }
                    }
                }
            }
            if (sWord.compare(data) == 0) {
                int nFeatures = Features.nFeatures;
                search_data = false;
                float feature;
                int ROWS = 0;
                while (!file.eof()) {
                    for (int COLS = 0; COLS < nFeatures; COLS++) {
                        file >> feature;
                        float rest = feature - ((int) feature);
                        if (rest > 0) {
                            feature = round(feature * 100.0 + 0.5) / 100.0;
                        }
                        mSamples[ROWS][COLS] = feature;
                    }
                    ROWS++;
                }
            }
        }
        file.close();
        count_instances(mSamples, mClasses, Features);
        show_instances(mClasses, INST, 2);
        show_data(mSamples, ROWS, COLS);
    }
}

void show_data(float **mSamples, int ROWS, int COLS) {
    cout << "\t---------- Dataset ----------" << endl;
    for (int row = 0; row < ROWS; row++) {
        cout << "[" << row << "]" << "\t";
        for (int col = 0; col < COLS; col++) {
            if (col == COLS - 1) {
                cout << mSamples[row][col];
            } else {
                cout << mSamples[row][col] << "\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void count_instances(float **mSamples, float **mClasses, oData Features) {
    int nSamples = Features.nSamples;
    int nInstances = Features.nClasses;
    int nFeatures = Features.nFeatures;
    int oClass, count;
    bool counted[nSamples] = {};
    for (int ins = 0; ins < nInstances; ins++) {
        oClass = mClasses[ins][0];
        count = 0;
        for (int i = 0; i < nSamples; i++) {
            if (counted[i]) {
                continue;
            } else {
                if (mSamples[i][nFeatures - 1] == oClass) {
                    count++;
                    counted[i] = true;
                }
            }
        }
        mClasses[ins][1] = count;
    }
    //write_data(mClasses, Features, 2);
    show_instances(mClasses, nInstances, 2);
}

void show_instances(float **mInstances, int ROWS, int COLS) {
    cout << "----- Instances -----" << endl;
    for (int row = 0; row < ROWS; row++) {
        cout << "[" << row << "]" << "\t";
        for (int col = 0; col < COLS; col++) {
            if (col == COLS - 1) {
                cout << mInstances[row][col];
            } else {
                cout << mInstances[row][col] << "\t";
            }
        }
        cout << endl;
    }
}

void select_random(float **mSamples, float **mClasses, float **mRandom, oData Features) {
    int ROWS = Features.nSamples;
    int COLS = Features.nFeatures;
    int CLSS = Features.nClasses;
    int RNDM = Features.nRandom;
    int NSPI = Features.nSPI;
    bool Selected[ROWS] = {};
    float current_class;
    int randIndex;
    int count = 0;
    int instances;

    for (int sClass = 0; sClass < CLSS; sClass++) {
        current_class = mClasses[sClass][0];
        instances = 0;
        while (instances < NSPI) {
            randIndex = rand() % ROWS;
            if (!Selected[randIndex]) {
                if (mSamples[randIndex][COLS - 1] == current_class) {
                    for (int i = 0; i < COLS; i++) {
                        mRandom[count][i] = mSamples[randIndex][i];
                    }
                    mRandom[count][COLS] = randIndex;
                    mRandom[count][COLS + 1] = mClasses[sClass][1];
                    Selected[randIndex] = true;
                    instances++;
                    count++;
                }
            }
        }
    }
    //write_data(mRandom, Features, 4);
    show_random(mRandom, RNDM, COLS + 3);
}

void show_random(float **mRandom, int ROWS, int COLS) {
    cout << "\t---------- Randomly Selected Samples ----------" << endl;
    for (int i = 0; i < ROWS; i++) {
        cout << "[" << i << "]" << "\t";
        for (int j = 0; j < COLS; j++) {
            if (j == COLS - 1) {
                cout << mRandom[i][j];
            } else {
                cout << mRandom[i][j] << "\t";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void nearest_neighbor(float **mSamples, float **mRandom, oData Features) {
    int nFeatures = Features.nFeatures;
    int nSamples = Features.nSamples;
    int nRandom = Features.nRandom;
    float **vDistances = new float*[nSamples - 1];
    for (int i = 0; i < (nSamples - 1); i++) {
        vDistances[i] = new float[2];
    }
    float vRandom[nFeatures + 2] = {};
    float vSample[nFeatures] = {};
    float current_class;
    int rand_row = 0;
    float vDistance;
    int grSize;
    int K;
    while (rand_row < nRandom) {
        int count = 0;
        toArray(mRandom, vRandom, rand_row, nFeatures + 2);
        current_class = vRandom[nFeatures - 1];
        for (int i = 0; i < nSamples; i++) {
            toArray(mSamples, vSample, i, nFeatures);
            if (vRandom[nFeatures] == i) {
                continue;
            }
            vDistance = Euclidean(vRandom, vSample, nFeatures - 1);
            vDistances[count][0] = vDistance;
            vDistances[count][1] = vSample[nFeatures - 1];
            count++;
        }
        sort_data(vDistances, nSamples - 1);
        grSize = vRandom[nFeatures + 1];
        K = select_K(vDistances, current_class, grSize);
        mRandom[rand_row][nFeatures + 2] = K;
        rand_row++;
    }
}

void toArray(float **mSamples, float *vSample, int Row, int Col) {
    for (int i = 0; i < Col; i++) {
        vSample[i] = mSamples[Row][i];
    }
}

float Euclidean(float *rVector, float *sVector, int Size) {
    float Sum, Dist, X, Y, S;
    Sum = 0;
    for (int i = 0; i < Size; i++) {
        X = rVector[i];
        Y = sVector[i];
        S = X - Y;
        S = pow(S, 2);
        Sum = Sum + S;
    }
    Dist = sqrt(Sum);
    return Dist;
}

void sort_data(float **vDistances, int Rows) {
    int min_row;
    float dist;
    float clss;
    for (int i = 0; i < Rows - 1; i++) {
        min_row = i;
        for (int j = i + 1; j < Rows; j++) {
            if (vDistances[j][0] < vDistances[min_row][0]) {
                min_row = j;
            }
        }
        dist = vDistances[min_row][0];
        clss = vDistances[min_row][1];
        vDistances[min_row][0] = vDistances[i][0];
        vDistances[min_row][1] = vDistances[i][1];
        vDistances[i][0] = dist;
        vDistances[i][1] = clss;
    }
}

void show_distances(float **vDistances, int Rows) {
    cout << "--- Distances ---" << endl;
    for (int i = 0; i < Rows; i++) {
        cout << "[" << i << "]\t"
                << vDistances[i][0] << " "
                << vDistances[i][1] << endl;
    }
}

int select_K(float **mDistances, float sClass, int gSize) {
    int nNumbers;
    int success;
    int Kmin = 5;
    float weight;
    float impact;
    float sAccuracy;
    float sTolerance;
    int Kmax = round(gSize * 0.90);
    if (Kmax % 2 == 0) {
        Kmax = getOdd(Kmax);
    }
    nNumbers = getCount(Kmin, Kmax);
    float weights[nNumbers];
    int kvalues[nNumbers];
    //float results[10];
    int index = 0; //cont;
    cout << "\t\t----- New Sample -----" << endl;
    cout << "----- Values -----" << endl;
    cout << "Class:\t" << sClass << endl;
    cout << "Group Size:\t" << gSize << endl;
    cout << "Max K value:\t" << Kmax << endl;
    cout << "N numbers:\t" << nNumbers << endl;
    //results[0] = sClass;
    //results[1] = gSize;
    //results[2] = Kmax;
    //results[3] = nNumbers;
    //write_results(results,0,1);
    cout << "\t--------- Measures ---------" << endl;
    while (Kmin <= Kmax) {
        success = 0;
        //cont = 4;
        //results[cont] = Kmin;
        for (int i = 0; i < Kmin; i++) {
            if (mDistances[i][1] == sClass) {
                success++;
            }
        }
        //results[cont++] = success;
        impact = logf(Kmin);
        sAccuracy = (float) success / Kmin;
        sTolerance = (float) (success - 1) / Kmin;
        weight = impact * sAccuracy + impact * sTolerance;

        impact = round(impact * 100.0) / 100.0;
        weight = round(weight * 100.0) / 100.0;
        sAccuracy = round(sAccuracy * 100.0) / 100.0;
        sTolerance = round(sTolerance * 100.0) / 100.0;
        cout << "[" << Kmin << "]" << "\t"
                " Success: " << success << "\t"
                " Accuracy: " << sAccuracy << "\t"
                " Tolerance: " << sTolerance << "\t"
                " Impact: " << impact << "\t"
                " Weight: " << weight << endl;
        //results[cont++] = sAccuracy;
        //results[cont++] = sTolerance;
        //results[cont++] = impact;
        //results[cont++] = weight;
        kvalues[index] = Kmin;
        Kmin += 2;
        weights[index] = weight;
        index++;
        //write_results(results,10,2);
    }
    float KWeight = 0;
    int K;
    for (int i = 0; i < nNumbers; i++) {
        if (weights[i] > KWeight) {
            KWeight = weights[i];
            K = kvalues[i];
        }
    }
    cout << "Selected K: " << K << " Value: " << KWeight << endl;
    cout << endl;
    return K;
}

int getOdd(int N) {
    while (N % 2 == 0) {
        N++;
    }
    return N;
}

int getCount(int Min, int Max) {
    int count = 0;
    while (Min <= Max) {
        count++;
        Min += 2;
    }
    return count;
}

float standardDeviation(int Size, float Mean, float *vAccuracy) {
    float StDev, Sum = 0;
    for (int i = 0; i < Size; i++) {
        Sum += powf((vAccuracy[i] - Mean), 2);
    }
    StDev = Sum / (Size - 1);
    StDev = sqrtf(StDev);
    return StDev;
}

float Manhattan(float *cVector, float *sVector, int Size) {
    float Sum, X, Y, S;
    Sum = 0;
    for (int i = 0; i < Size; i++) {
        X = cVector[i];
        Y = sVector[i];
        S = X - Y;
        S = abs(S);
        Sum = Sum + S;
    }
    return Sum;
}

void write_data(float **Data, oData Features, int Mode) {
    ofstream file;
    switch (Mode) {
        case 1:
            file.open(RFILE, ios::out);
            file << "Total Samples:  " << Features.nSamples << endl;
            file << "Total Features: " << Features.nFeatures << endl;
            file << "Total Classes:  " << Features.nClasses << endl;
            file << "Total Random:   " << Features.nRandom << endl;
            file << "Samples per Instance: " << Features.nSPI << endl;
            file.close();
            break;
        case 2:
            file.open(RFILE, ios::out | ios::app);
            file << "@Instances" << endl;
            for (int i = 0; i < Features.nClasses; i++) {
                file << "[" << i << "] " << Data[i][0] << " " << Data[i][1] << endl;
            }
            file.close();
            break;
        case 3:
            file.open(RFILE, ios::out | ios::app);
            file << "@Data" << endl;
            for (int i = 0; i < Features.nSamples; i++) {
                file << "[" << i << "] ";
                for (int j = 0; j < Features.nFeatures; j++) {
                    file << Data[i][j] << " ";
                }
                file << endl;
            }
            file.close();
            break;
        case 4:
            file.open(RFILE, ios::out | ios::app);
            file << "@Random" << endl;
            for (int i = 0; i < Features.nRandom; i++) {
                file << "[" << i << "] ";
                for (int j = 0; j < Features.nFeatures + 3; j++) {
                    file << Data[i][j] << " ";
                }
                file << endl;
            }
            file.close();
            break;
    }
}

void write_results(float *Data, int Cols, int Mode) {
    ofstream file;
    file.open(RFILE, ios::out | ios::app);
    switch (Mode) {
        case 1:
            file << "@Values" << endl;
            file << "Class:       " << Data[0] << endl;
            file << "Group Size:  " << Data[1] << endl;
            file << "Max K Value: " << Data[2] << endl;
            file << "K Numbers:   " << Data[3] << endl;
            file << "@Measures" << endl;
            break;
        case 2:
            for (int i = 4; i < Cols; i++) {
                file << Data[i] << " ";
            }
            file << endl;
            break;
    }
    file.close();
}

void getFold(float **Dataset, float *Fold, int ROW, int COLS) {
    
}