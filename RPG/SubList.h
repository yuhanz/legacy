//---------- functions for 'Merge Sort' ----------
#include <iostream.h>
//----------------- object 'SubList' -----------------------------
/*  (Note: 'SubList' is part of a contiguous list, which keeps
--         the whole list's address and its starting index and
--         ending index. (The data in the ending index and
--          starting index are included in the sublist.
--              A sublist also knows how to break into two length
--        equavalent sublists.                                   )
-----------------------------------------------------------------*/
template<class Data>
class SubList
             {
              protected:
                Data* list;
                int starting;
                int ending;
              public:
                SubList();
                SubList(Data*,int);
                SubList(Data*,int,int);
                int getSize();
                SubList getLeftHalf();
                SubList getRightHalf();
                void merge(SubList&,SubList&);
                void sort();
             };

//-------------- members of 'Sublist' -------------
   //--- constructor ---
    template<class Data>
    SubList<Data>::SubList()        // use this one to recieve an returned Sublist.
    {}
    template<class Data>
    SubList<Data>::SubList(Data* l, int s, int e) : list(l), starting(s), ending(e)
    {}
    template<class Data>
    SubList<Data>::SubList(Data* l, int size) : list(l), starting(0), ending(size-1)
    {}

    //--- functions ---
    template<class Data>
    int SubList<Data>::getSize() { return ending-starting+1; }
    template<class Data>
    SubList<Data> SubList<Data>::getLeftHalf()
    {
     int size = getSize();
     if(size<=1)
        {
         cout << "\nError: list is too small to split.";
         exit(0);
        }
     return SubList<Data>(list, starting, starting + size/2 - 1);
    }

    template<class Data>
    SubList<Data> SubList<Data>::getRightHalf()
    {
     int size = getSize();
     if(size<=1)
        {
         cout << "\nError: list is too small to split.";
         exit(0);
        }
     return SubList<Data>(list, starting + size/2 , ending);
    }


    //+++ merge two sublist back together and store in this list.
    template<class Data>
    void SubList<Data>::merge(SubList<Data>& left, SubList<Data>& right)
    {
	 Data* tempList= new Data[getSize()];        // a buffer, whose size equals to this sublist.;

     int index=0;

     while(left.getSize()>0 && right.getSize()>0)
       {
        if(left.list[left.starting] <= right.list[right.starting])
          {
           tempList[index] = left.list[left.starting];
           left.starting++;
          }
        else
          {
           tempList[index] = right.list[right.starting];
           right.starting++;
          }
        index++;
       }

              // Insert the rest of the left list to the end of the whole list.
              //  (If the right list is not empty, it's been already there.
              //   It's because right half of the list is at the end of the
              //   whole list.)
    if(right.getSize()==0)
      {
       int listIndex = ending-left.getSize()+1;
       int leftIndex = left.starting;
       for(int i=0;i<left.getSize();i++)
          list[listIndex+i] = left.list[leftIndex+i];
      }

    for(int i=0;i<index;i++)
      list[starting+i]=tempList[i];

	delete[] tempList;
	
    }

    //+++ sort a sublist using merge sort +++
    template<class Data>
    void SubList<Data>::sort()
    {
     if(getSize()<=1)
        return;

     SubList<Data> left  = getLeftHalf();
     SubList<Data> right = getRightHalf();

     left.sort();
     right.sort();

     merge(left,right);

     return;
    }


