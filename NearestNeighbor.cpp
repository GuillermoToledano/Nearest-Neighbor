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

struct oFile {
    int nSamples;
    int nFeatures;
    int nInstances;
    int nRandom;
};

// Prototypes
oFile count_samples();
void read_data(oSample *samplesVector);
void show_data(oSample *sVector, int size);
void select_random(oSample *sVector, oRSample *rSamples, oFile Features);
void show_random(oRSample *rSelected, int nRandom);
void calculate_K(oSample *sVector, oRSample *rSamples, oFile Features);
void toArray(oSample iSample, int *sVector);
int Euclidean(int *rVector, int *sVector, int Features);
void sort_data(oDist *vDistances, int Size);
void show_distances(oDist *vDistances, int K);
int select_k(oDist *vDistances, oClass cSample, oFile Features);
int getOdd(int N);
int getCount(int Min, int Max);
float standardDeviation(int Size, float Mean, float *vAccuracy);
// Variables

int main(int argc, char** argv) {
    oFile Features;
    Features = count_samples();
    int SIZE = Features.nSamples;
    int FEATURES = Features.nFeatures;
    cout << "Total Samples:\t" << SIZE << endl;
    cout << "Total Features:\t" << FEATURES << endl;
    int RANDOM = SIZE * 0.10;
    Features.nRandom = RANDOM;
    cout << "Random Samples:\t" << RANDOM << endl;

    oSample Dataset[SIZE], randomSamples[RANDOM], rDataset[(SIZE - RANDOM)];
    oRSample rSamples[RANDOM];
    oSample Train, Test;
    oKSample kRandomSamples[RANDOM];

    read_data(Dataset);
//    cout << sizeof(Dataset)/sizeof(*Dataset) << endl;
    //Show_data(Dataset);
    select_random(Dataset, rSamples, Features);
    //Show_random(randomSamples);
    calculate_K(Dataset, rSamples);
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

void select_random(oSample *sVector, oRSample *rSamples, oFile Features) {
    int Size = Features.nSamples;
    int Rand = Features.nRandom;
    bool rSelected[Size] = {};
    oSample randomSelected;
    oRSample randSelected;
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
                    randSelected.Sample = randomSelected;
                    randSelected.Index = index;
                    rSamples[count] = randSelected;
                    rSelected[index] = true;
                    instances++;
                    count++;
                }
            }
        }
    }
    show_random(rSamples, Rand);
    cout << endl;
}

void show_random(oRSample *rSelected, int nRandom) {
    cout << "\t\t---------- Selected samples ----------" << endl;
    cout << "N \t" << "Radio\t" << "Ratio\t" << "White\t" << "Black\t" << 
            "BW\t" << "Green\t" << "C\t" << "Index" << endl;
    for (int i = 0; i < nRandom; i++) {
        cout << "[" << i << "] \t" <<
                rSelected[i].Sample.Features.Radio << "\t" <<
                rSelected[i].Sample.Features.Ratio << "\t" <<
                rSelected[i].Sample.Features.White << "\t" <<
                rSelected[i].Sample.Features.Black << "\t" <<
                rSelected[i].Sample.Features.BoutG << "\t" <<
                rSelected[i].Sample.Features.Green << "\t" <<
                rSelected[i].Sample.Class << "\t" <<
                rSelected[i].Index << endl;
    }
    cout << endl;
}

void calculate_K(oSample *sVector, oRSample *rSamples, oFile Features) {
    int oSize = Features.nSamples;
    int oFTS = Features.nFeatures;
    int oRND = Features.nRandom;
    oDist vDistances[(oSize - 1)], cDist;
    oRSample cRandom, cSample;
    int vRandom[oFTS], vSample[oFTS];
    int KValue, sDistance;
    oKSample auxSample;
    for (int x = 0; x < oRND; x++) {
        cRandom = rSamples[x];
        toArray(cRandom.Sample, vRandom);
        for (int y = 0; y < oSize - 1; y++) {
            cSample.Sample = sVector[y];            
            toArray(cSample.Sample, vSample);
            sDistance = Euclidean(vRandom, vSample, oFTS);
            if (sDistance == 0 && cRandom.Index == y) {
                continue;
            }
            cDist.Distance = sDistance;
            cDist.Class = cSample.Sample.Class;
            vDistances[y] = cDist;
        }
        sort_data(vDistances, oSize);
        KValue = select_k(vDistances, cRandom.Sample.Class);
        //auxSample.Sample = cRandom;
        //auxSample.K = kValue;
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

int Euclidean(int *rVector, int *sVector, int Features) {
    int Sum, Dist, X, Y, S;
    Sum = 0;
    for (int i = 0; i < Features - 1; i++) {
        X = rVector[i];
        Y = sVector[i];
        S = X - Y;
        S = pow(S, 2);
        Sum = Sum + S;
    }
    Dist = sqrt(Sum);
    return Dist;
}

void sort_data(oDist *vDistances, int Size) {
    oDist auxDist;
    int min_index;
    for (int i = 0; i < (Size - 2); i++) {
        min_index = i;
        for (int j = i + 1; j < (Size - 1); j++) {
            if (vDistances[j].Distance < vDistances[min_index].Distance) {
                min_index = j;
            }
        }
        auxDist = vDistances[i];
        vDistances[i] = vDistances[min_index];
        vDistances[min_index] = auxDist;
    }
//    show_distances(vDistances, (SZE - 1));
}

void show_distances(oDist *vDistances, int K) {
    cout << "----- Sorted Distances -----" << endl;
    for (int i = 0; i < K; i++) {
        cout << "[" << i << "] " <<
                "Distance: " << vDistances[i].Distance << "\t"
                " Class: " << vDistances[i].Class << "\t" << endl;
    }
}

int select_k(oDist *vDistances, oClass cSample, oFile Features) {
    int Size = Features.nSamples;
    int Rand = Features.nRandom;
    int K = 5;
    float cTolerance;
    int success, correct;
    float cAccuracy, sAccuracy;
    int Kmax = round((Size - Rand) * 0.20);
    cout << "\t\t----- New Sample -----" << endl;
    cout << "----- Values -----" << endl;
    cout << "Class:\t" << cSample << endl;
    cout << "Max K value:\t" << Kmax << endl;
    if (Kmax % 2 == 0) {
        Kmax = getOdd(Kmax);
    }
    cout << "Max K value:\t" << Kmax << endl;
    int auxK = K;
    int nSize = getCount(auxK, Kmax);
    cout << "N numbers:\t" << nSize << endl;
    float accuracy[nSize], tolerance[nSize];
    float greatest = 0;
    float sum = 0;
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
        sAccuracy = (float)success / K;
        cAccuracy = (float)correct / K;
        cTolerance = ((float) correct - 1) / K;
        if (cAccuracy > greatest) {
            greatest = cAccuracy;
        }
        sum += cAccuracy;
        accuracy[count] = cAccuracy;
        tolerance[count] = cTolerance;
        cout << "[" << K << "]" <<
                " Correct:\t" << correct <<
                " Success:\t" << success <<
                " Accuracy:\t" << cAccuracy <<
                " Tolerance:\t" << cTolerance << endl;
        count++;
        K += 2;
    }
    float mean = sum / nSize;
    float stDev = standardDeviation(nSize, mean, accuracy);
    cout << "Greatest Value:\t" << greatest << endl;
    cout << "Sum:\t" << sum << endl;
    cout << "Mean:\t" << mean << endl;
    cout << "St Dev:\t" << stDev << endl;
    float Thrs = greatest - stDev * 2;
    cout << "Thrs:\t" << Thrs << endl;
    
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