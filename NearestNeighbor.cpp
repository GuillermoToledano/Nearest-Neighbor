/* 
 * File:   main.cpp
 * Author: GuillermoToledano
 *
 * Created on February 26, 2019, 12:16 PM
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>
#define SZE 170
#define RND 18
#define SPI 3 
#define CLS 6
#define FTS 7
#define SHW 5

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

/** 
 * Object Sample K
 * @Sample From oSample struct
 * @K Assigned K value 
 */
struct oSampleK {
    oSample Sample;
    int K;
};

struct oDist {
    oClass Class;
    int Distance;
};

struct oFile {
    int nSamples;
    int nFeatures;
    int nInstances;
};

// Prototypes
oFile count_samples();
void read_data(oSample *samplesVector);
void show_data(oSample *sVector, int size);
void select_random(oSample *sVector, oSample *rSamples, bool *rSelected);
void show_random(oSample *rSelected);
void calculate_K(oSample *sVector, oSample *rSamples);
void toArray(oSample iSample, int *sVector);
int Euclidean(int *rVector, int *sVector);
void sort_data(oDist *vDistances);
void show_distances(oDist *vDistances);
int select_k(oDist *vDistances, oClass cSample);
int getOdd(int n);
int getCount(int min, int max);
// Variables

int main(int argc, char** argv) {
    oFile File;
    File = count_samples();
    int SIZE = File.nSamples;
    int FEATURES = File.nFeatures;
    cout << "Total Samples:\t" << SIZE << endl;
    cout << "Total Features:\t" << FEATURES << endl;
    int RANDOM = SIZE * 0.10;
    cout << "Random Samples:\t" << RANDOM << endl;

    oSample Dataset[SZE], randomSamples[RND], rDataset[(SZE - RND)];
    oSample Train, Test;
    oSampleK kRandomSamples[RND];
    bool rSelected[SZE] = {};

    read_data(Dataset);
    //Show_data(Dataset);
    select_random(Dataset, randomSamples, rSelected);
    //Show_random(randomSamples);
    calculate_K(rDataset, randomSamples);
    return 0;
}

oFile count_samples() {
    oFile outFile;
    ifstream file("Data Files/Samples_.txt");
    string line;
    int count = 0;
    int features = 0;
    bool flag = false;
    while (getline(file, line)) {
        if (!flag) {
            for (char c : line) {
                if (c == ' ') {
                    features++;
                }
            }
            flag = true;
        }
        count++;
    }

    outFile.nSamples = count;
    outFile.nFeatures = features + 1;
    return outFile;
}

void read_data(oSample *samplesVector) {
    double radio, ratio, white, black, green, boutg;
    int type;
    oSample input;
    int index = 0;
    ifstream file;

    file.open("Data Files/Samples_.txt", ios::in);
    if (file.is_open()) {
        cout << "Reading data..." << endl;
        while (!file.eof()) {
            if (file.eof()) {
                break;
            }

            file >> radio;
            file >> ratio;
            file >> white;
            file >> black;
            file >> boutg;
            file >> green;
            file >> type;

            input.Features.Radio = radio;
            input.Features.Ratio = floor((ratio * 100 + 0.5)) / 100;
            input.Features.White = floor((white * 100 + 0.5)) / 100;
            input.Features.Black = floor((black * 100 + 0.5)) / 100;
            input.Features.BoutG = floor((boutg * 100 + 0.5)) / 100;
            input.Features.Green = floor((green * 100 + 0.5)) / 100;
            switch (type) {
                case 1:
                    input.Class = oBall1;
                    break;
                case 2:
                    input.Class = oBall2;
                    break;
                case 3:
                    input.Class = oCD1;
                    break;
                case 4:
                    input.Class = oCD2;
                    break;
                case 5:
                    input.Class = oCD3;
                    break;
                case 6:
                    input.Class = oBall6;
                    break;
            }
            samplesVector[index] = input;
            index++;

        }
        file.close();
        show_data(samplesVector, (index - 1));
        cout << endl;
    } else {
        cout << "Something wrong!..." << endl;
    }
}

void show_data(oSample *samplesVector, int Size) {
    cout << "                ---------- Dataset ----------" << endl;
    for (int i = 0; i < Size; i++) {
        cout << "[" << i << "] \t" <<
                samplesVector[i].Features.Radio << "\t" <<
                samplesVector[i].Features.Ratio << "\t" <<
                samplesVector[i].Features.White << "\t" <<
                samplesVector[i].Features.Black << "\t" <<
                samplesVector[i].Features.BoutG << "\t" <<
                samplesVector[i].Features.Green << "\t" <<
                samplesVector[i].Class << endl;
    }
    cout << endl;
}

void select_random(oSample *sVector, oSample *rSamples, bool *rSelected) {
    oSample randomSelected;
    //    srand((int)time(0));
    int index;
    int count = 0;
    int instances;
    for (int oClass = 1; oClass <= CLS; oClass++) { // CLS = 6
        instances = 0;
        while (instances < SPI) { // SPI = 3
            index = rand() % 170; // 0 >= index < 170
            if (!rSelected[index]) {
                randomSelected = sVector[index];
                if (randomSelected.Class == oClass) {
                    rSamples[count] = randomSelected;
                    rSelected[index] = true;
                    instances++;
                    count++;
                }
            }
        }
    }
    show_random(rSamples);
    cout << endl;
}

void show_random(oSample *rSelected) {
    cout << "          ---------- Selected samples ----------" << endl;
    for (int i = 0; i < RND; i++) {
        cout << "[" << i << "] \t" <<
                rSelected[i].Features.Radio << "\t" <<
                rSelected[i].Features.Ratio << "\t" <<
                rSelected[i].Features.White << "\t" <<
                rSelected[i].Features.Black << "\t" <<
                rSelected[i].Features.BoutG << "\t" <<
                rSelected[i].Features.Green << "\t" <<
                rSelected[i].Class << endl;
    }
    cout << endl;
}

void calculate_K(oSample *sVector, oSample *rSamples) {
    oDist vDistances[SZE], cDist;
    oSample cRandom, cSample;
    int vRandom[FTS], vSample[FTS];
    int kValue;
    oSampleK auxSample;
    for (int x = 0; x < RND; x++) {
        cRandom = rSamples[x];
        for (int y = 0; y < SZE; y++) {
            cSample = sVector[y];
            toArray(cRandom, vRandom);
            toArray(cSample, vSample);
            cDist.Distance = Euclidean(vRandom, vSample);
            cDist.Class = cSample.Class;
            vDistances[y] = cDist;
        }
        sort_data(vDistances);
        kValue = select_k(vDistances, cRandom.Class);
        auxSample.Sample = cRandom;
        auxSample.K = kValue;
    }
}

void toArray(oSample iSample, int *sVector) {
    sVector[0] = iSample.Features.Radio;
    sVector[1] = iSample.Features.Ratio;
    sVector[2] = iSample.Features.White;
    sVector[3] = iSample.Features.Black;
    sVector[4] = iSample.Features.BoutG;
    sVector[5] = iSample.Features.Green;
    sVector[6] = iSample.Class;
}

int Euclidean(int *rVector, int *sVector) {
    int Sum, Dist, X, Y, S;
    Sum = 0;
    for (int i = 0; i < FTS - 1; i++) {
        X = rVector[i];
        Y = sVector[i];
        S = X - Y;
        S = pow(S, 2);
        Sum = Sum + S;
    }
    Dist = sqrt(Sum);
    return Dist;
}

void sort_data(oDist *vDistances) {
    oDist auxDist;
    int min_index;
    for (int i = 0; i < SZE - 1; i++) {
        min_index = i;
        for (int j = i + 1; j < SZE; j++) {
            if (vDistances[j].Distance < vDistances[min_index].Distance) {
                min_index = j;
            }
        }
        auxDist = vDistances[i];
        vDistances[i] = vDistances[min_index];
        vDistances[min_index] = auxDist;
    }
}

void show_distances(oDist *vDistances, int K) {
    cout << "----- Sorted Distances -----" << endl;
    for (int i = 0; i < K; i++) {
        cout << "[" << i << "] " <<
                "Distance: " << vDistances[i].Distance << "\t"
                " Class: " << vDistances[i].Class << "\t" << endl;
    }
}

int select_k(oDist *vDistances, oClass cSample) {
    int size;
    int K = 5;
    double cAccuracy, sAccuracy;
    int success, correct;
    int Kmax = round((SZE - RND) * 0.20);
    cout << "\t\t----- New Sample -----" << endl;
    cout << "----- Values -----" << endl;
    cout << "Class:\t" << cSample << endl;
    cout << "Max K value:\t" << Kmax << endl;
    if (Kmax % 2 == 0) {
        Kmax = getOdd(Kmax);
    }
    cout << "Max K value:\t" << Kmax << endl;
    int auxK = K;
    size = getCount(auxK, Kmax);
    cout << "N numbers:\t" << size << endl;
    double pMeasures[size];
    int count = 0;
    cout << "\t\t--------- Measures ---------" << endl;
    while (K <= Kmax) {
        success = 0;
        correct = ceil(K * 0.75);
        for (int i = 0; i < K; i++) {
            if (vDistances[i].Class == cSample) {
                success++;
            }
        }
        sAccuracy = (double) success / K;
        cAccuracy = (double) correct / K;
        if (cAccuracy > 1) {
            cAccuracy = floor(cAccuracy);
        }
        pMeasures[count] = cAccuracy;
        cout << "[" << K << "]" <<
                " Correct:\t" << correct <<
                " Success:\t" << success <<
                " Accuracy:\t" << cAccuracy <<
                " Tolerance:\t" << ((float) correct - 1) / K << endl;
        count++;
        K += 2;
    }
    cout << "Count:\t" << count << endl;
    K = 5;
    for (int i = 0; i < size; i++) {
        if (pMeasures[i] >= 0.60) {
            cout << "Selected K:\t" << K << endl;
            show_distances(vDistances, K);
            cout << endl;
            cout << endl;
            return K;
        }
        K += 2;
    }
    K = 1;
    cout << "Selected K:\t" << K << endl;
    show_distances(vDistances, K);
    cout << endl;
    cout << endl;
    return K;
}

int getOdd(int n) {
    while (n % 2 == 0) {
        n++;
    }
    return n;
}

int getCount(int min, int max) {
    int count = 0;
    while (min <= max) {
        count++;
        min += 2;
    }
    return count;
}