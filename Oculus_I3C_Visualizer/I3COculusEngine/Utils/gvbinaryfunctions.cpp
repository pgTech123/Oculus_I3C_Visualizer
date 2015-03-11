#include "gvbinaryfunctions.h"

int numberHighBits(unsigned char ucNumber)
{
    int iCount = 0;
    unsigned char ucComparator = 0x01;

    for(int i = 0; i < sizeof(unsigned char)*8; i++)
    {
        if((ucNumber & ucComparator) != 0)
        {
            iCount ++;
        }
        ucComparator = ucComparator << 1;
    }

    return iCount;
}

int numberHighBits(int iNumber)
{
    int iCount = 0;
    int ucComparator = 0x01;

    for(int i = 0; i < sizeof(int)*8; i++)
    {
        if((iNumber & ucComparator) != 0)
        {
            iCount ++;
        }
        ucComparator = ucComparator << 1;
    }

    return iCount;
}

bool isBase2(int iNumber)
{
    if(numberHighBits(iNumber) == 1)
    {
        return true;
    }
    return false;
}

int firstHighBit(int iNumber)
{
    int ucComparator = 0x01;

    for(int i = 0; i < sizeof(int)*8; i++)
    {
        if((iNumber & ucComparator) != 0)
        {
            return i;
        }
        ucComparator = ucComparator << 1;
    }

    return 0;
}

bool isBitHigh(unsigned char ucNumber, unsigned char ucBit)
{
    unsigned char cmp = 0x01 << ucBit;

    if((cmp & ucNumber) != 0)
    {
        return true;
    }
    return false;

}

void sort(double dUnsortedArray[8], unsigned char uc_IndexSorted[8])
{
    //To optimize (TODO)

    bool bArrAlreadySorted[8];
    double tmp = 2000000;
    double potentialNextV = 2000000;
    unsigned char index = 0;
    //initialize
    for(int i = 0; i < 8; i++)
    {
        bArrAlreadySorted[i] = false;
    }

    //Sort
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(!bArrAlreadySorted[j])
            {
                tmp = dUnsortedArray[j];

            }
            if(tmp < potentialNextV)
            {
                potentialNextV = tmp;
                index = j;
            }
        }
        uc_IndexSorted[i] = index;
        bArrAlreadySorted[index] = true;
        potentialNextV = 2000000;
        tmp = 2000000;
    }
}
