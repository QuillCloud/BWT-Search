//
//  bwtsearch.cpp
//  assignment 2
//
//  Created by Zhang Yunhe on 17/04/2017.
//  Copyright © 2017 Zhang Yunhe. All rights reserved.
//

#include "bwtsearch.h"

/*
 Consider the memory limitation, divide bwt file into blocks;
 
 Let each block have 18000 characters, since bwt file smaller than 160MB,
 
 the block number will be 160*1024*1024/18000 + 1;
 
 Set global varibale:
 
 "k" to 18000;
 
 "block" to 160*1024*1024/k + 1;
 
 "occ_array[block][127]": store information about occ
 columns represent 127 different characters, rows represent each blocks
 
 C_Map: store information of the C[c], map c to C[c];
 
 C_Next: similar to C_Map, map c to C[c+1];
 
 */
const int k = 18000;
const int block = 160*1024*1024/k + 1;
char read[k];
unsigned long l_of_read;
int occ_array[block][127];
map<int, int> C_Map;
map<int, int> C_Next;
FILE * BWT;

int main(int argc, char * argv[]) {
    /*
     make sure number of arguments is in range [3, 5]
     bwt file; index file; 1-3 terms;
     */
    if (argc <= 3 or argc > 6) {
        return 0;
    }
    
    /*
     Variables initialization.
     */
    
    //BWT, ch: for reading the bwt file.
    BWT = fopen(argv[1], "r");
    if (!BWT) {
        return 0;
    }
    
    /*
     i, j: use in for loop;
     column, row: location of "occ_array";
     count: for counting;
     */
    int i, j, column;
    int row = 0;
    int count = 0;
    
    /*
     Aim:
     
     Create the Occ(c,q): number of occurrence of char c in prefix L[1,q];
     
     Use 2D-array "occ_array" to contain the information of Occ(c,q);
     
     Columns represent the characters(transformed by function conver_ascii_index);
     
     Rows contain the occ value in prefix [1,k], [1,2*k], [1,3*k] and so forth,
     Each row represent the number of occurrence of c in prefix L[1, (row number)*k].
     
     
     Process:
     
     Read the bwt file, then create the "occ_array";
     
     Index file not exists, read bwt file to create the "occ_array";
     
     Read character one by one and update the value in corresponding location of "occ_array";
     
     Change row for every k(18000) characters.
     
     No index file, because when I using index file, it didn't save time very much but increased the memroy.

    */
    while(1) {
        l_of_read = fread(read, sizeof(char), k, BWT);
        if (row > 0) {
            for (i = 0; i < 127; i++) {
                occ_array[row][i] = occ_array[row - 1][i];
            }
        }
        for (i = 0; i < l_of_read; i++) {
            column = (int)read[i];
            occ_array[row][column]++;
        }
        if(feof(BWT)) {
            break;
        }
        row++;
    }
    
    /*
     Aims:
     
     C[c] contains the total number of text chars in T which are alphabetically smaller then c
     
     Use map C_Map to contain the information of C[c], use c as the key, C[c] as the value
     
     C_Next, use c as key, C[c+1](c+1 means next character of c) as the value
     
     Process:
     
     Use last row of "occ_array" to create the C_Map and C_Next;
     */
    
    int previous = 0;
    count = 0;
    for (i = 0; i < 127; i++) {
        if (occ_array[row][i] != 0) {
            if (count == 0) {
                C_Map[i] = count;
            }
            else {
                C_Next[previous] = count;
                C_Map[i] = count;
            }
            previous = i;
            count += occ_array[row][i];
        }
    }
    C_Next[previous] = count;
    
    /*
        Array "terms" store every terms;
     
     */
    string terms[3] = {"", "", ""};
    
    for (i = 3; i < argc; i++) {
        terms[i-3] = argv[i];
    }
    const char * search;
    int longest_f_l[2] = {1, 0};
    int get = 0;
    int len = -1;
    int index;
    int c_num;
    int First;
    int Last;
    
    /*
        Using Backward Search Algorithm to find the each terms in bwt file.
        
        get each terms' "Last" and "First", since the number of results is Last - First;
     
        Consider less results may be faster in decoding <Text> parts,
     
        chose the term that has least results and save it's "Last" and "First" in array "longest_f_l";
     
     */
    for(i = 0; i <= argc - 4; i++) {
        search = terms[i].c_str();
        index = (int)strlen(search) - 1;
        c_num = (int)search[index];
        First = C_Map[c_num] + 1;
        Last = C_Next[c_num];
        while (index > 0 && Last >= First) {
            c_num = (int)search[index - 1];
            First = C_Map[c_num] + occurrence(c_num, First - 1) + 1;
            Last = C_Map[c_num] + occurrence(c_num, Last);
            index--;
        }
        if (i == 0) {
            len = Last - First;
            get = i;
            longest_f_l[0] = First;
            longest_f_l[1] = Last;
        }
        else if (Last - First < len) {
            len = Last - First;
            get = i;
            longest_f_l[0] = First;
            longest_f_l[1] = Last;
        }
    }
    
    /*
     Aims:
     
     Decoding the <recordID><Text> base on the reasult calculate by Backward Search Algorithm.
     
     Process:
     
     Get "First" and "Last" from array "longest_f_l", means chose term that has least results.
     
     Call "back_search" function to get the previous part of <recordID><Text> "s1" by backward search;
     
     Call "forward_search" function to get the posterior part of <recordID><Text> "s2" by forward search;

     String "result" combine the "s1" and "s2";
     
     "match" get <Text> part of result,then try to find the rest terms in "match";
    
     If all rest terms in the "match", output the "result".
     
     Addition:
     
     Consider none of the testcases will result in more than 5,000 matches for each search term.
     
     Use array "duplicate" to store the <recordID> that already find.
     
     After backward search,if find the <recordID> in array "duplicate", the forward search will not be processed.
     
     The program will be faster if search term result in many repeated matches or the reapeated match is very long.
     
     */
    int flag;
    int duplicate[5001];
    int D = 0;
    int recordID;
    int u;
    string s1, s2, result, match;
    First = longest_f_l[0];
    Last = longest_f_l[1];
    if (Last >= First) {
        for (i = First; i<=Last; i++) {
            s1 = back_search(i);
            if (s1 == "") {
                continue;
            }
            recordID =  atoi(s1.substr(1, s1.find("]") - 1).c_str());
            for (u = 0; u < D; u++) {
                if(duplicate[u] == recordID) s1 = "";
            }
            if (s1 == "") {
                continue;
            }
            duplicate[D] = recordID;
            D++;
            if (D >= 5001) break;
            s2 = forward_search(i, row);
            flag = 0;
            result = s1 + s2;
            match = result.substr(result.find("]") + 1);
            if (argc > 4) {
                for(j = 0; j <= argc - 4; j++) {
                    if (j == get) continue;
                    if (match.find(terms[j]) == match.npos ) {
                        flag = 1;
                    }
                }
            }
            if (flag == 0) {
                printf("%s\n", result.c_str());
            }
        }
    }
    
    fclose(BWT);
    return 0;
    
}

/*
 Aim:
 
 For calculating the occ(c_num, num);
 
 Process:
 
 k = 18000;
 
 1.Divide the "num" by k, get "n";
 
 2.If "n" larger than 0, count = occ_array[n-1][c_num]; otherwise count = 0;
 Now "count" store the occurrance of "c_num" in first k*n characters in bwt file;
 
 3.rest = rest - k*n, "rest" store the number of rest of characters that we need to read.
 
 4.Start read bwt file, start location is (n * k);
 
 5.Read all of the rest characters, count the occurences of "c_num", add it into count.
 
 6.Return "count" which is occ(c_num, num).
 
 */
int occurrence(int c_num, int num) {
    int n = num/k;
    int count = 0;
    int rest = num;
    if (n > 0) {
        count = occ_array[n - 1][c_num];
        rest = num - n*k;
    }
    fseek(BWT, n*k, SEEK_SET);
    fread(read, sizeof(char), k, BWT);
    int i;
    for (i = 0; i <rest; i++) {
        if (c_num == read[i]) {
            count++;
        }
    }
    return count;
}

/*
 Aim:
 
 Give the occ number "num" and character "c", find the location in bwt file;
 (At which location in bwt file, the occurrence of character "c" reach the number "num")
 
 Process:
 
 k=18000;
 
 1.In "occ_array",use binary search to find which block contain charater "c".
 
 2."count" means charaters already read at that block,
 "oc" means occurence of charater "c" at that block,
 Since in "occ_array", change block for every k characters,
 count = block*k, oc = occ_array[block][c];
 
 3.Start read bwt file, start location is (block * k);
 
 4.Keep read charater and count the number (update "count")
 also count the occurrence of character "c" (update "oc"),
 until "oc" reach the number "num";
 
 5."count" is the result, return "count".
 
 */
int inverse_occ(int c, int num, int row) {
    int count = 0;
    int oc = 0;
    int uper = row;
    int lower = 0;
    int i;
    while (1) {
        i = (uper + lower) >> 1;
        if (i == 0 and occ_array[i][c] >= num) {
            fseek(BWT, 0, SEEK_SET);
            count = 0;
            oc = 0;
            break;
        }
        else if (occ_array[i][c] < num and occ_array[i+1][c] >= num) {
            fseek(BWT, (i+1)*k, SEEK_SET);
            count = (i+1)*k;
            oc = occ_array[i][c];
            break;
        }
        else if (occ_array[i][c] >= num) {
            uper = i;
        }
        else if (occ_array[i+1][c] < num) {
            lower = i;
        }
    }
    l_of_read = fread(read, sizeof(char), k, BWT);
    for (i = 0; i < l_of_read; i++) {
        count++;
        if (c == read[i]) {
            oc++;
        }
        if (oc == num) {
            break;
        }
    }
    return count;
}

/*
 Aim:
 
 Do backward search to get previous part of <recordID><Text>.
 
 Process:
 
 "LF" is the location of character in bwt list;
 
 1.Use "LF"(number) to get the character "ch" from bwt list(bwt file), add it to the head of string "s".
 
 2.Get the occ(ch, LF) "occ" (call function "occurrence") and the C[c] "c" (use map C_Map);
 
 3.Use occ and c to calculate new "LF", LF = occ + c;
 
 4.Return to the step 1, until get the character "]";
 
 5.Set flag from 0 to 1, Return step 1, until get the character "["
 
 6.string "s" is the result, return "s".
 
 */
string back_search(int number) {
    int ch;
    int flag = 0;
    fseek(BWT, number - 1, SEEK_SET);
    ch = getc(BWT);
    string s = string(1, (char)ch);
    int c_num = ch;
    int occ = occurrence(c_num, number);
    int c = C_Map[c_num];
    int LF = occ + c;
    while (1) {
        if (c_num == 91) {
            if (flag == 0) {
                return "";
            }
            else {
                break;
            }
        }
        if (c_num == 93) {
            flag = 1;
        }
        fseek(BWT, LF - 1, SEEK_SET);
        ch = getc(BWT);
        s = string(1, (char)ch) + s;
        c_num = ch;
        occ = occurrence(c_num, LF);
        c = C_Map[c_num];
        LF = occ + c;
    }
    return s;
}

/*
 Aim:
 
 Do forward search to get the posterior part of <recordID><Text>.
 
 Process:
 
 "cur_num" is the location of character in sorted list.
 
 Since the C_Next maping the "c" to "C[c+1]", if "cur_num" >= C_Next[c-1] but < C_Next[c],
 it means "c" is the character at the location "cur_num" in sorted list .
 
 1.Use "cur_num"(number) to get the character "c" from sorted list(C[c]), add it to tail of string "s";
 
 2.Get occurrence number "occ_num", occ_num = cur_num - C[c-1](store in "pre_num")
 
 4.Use "occ_num" and "c" to do reverse occurrence operation(call "inverse_occ"), get new "cur_num"
 
 5.Return to step 1, until reach character "[";
 
 6.string "s" is the result, return "s".
 */

string forward_search(int number, int row) {
    map<int,int>::iterator it;
    it = C_Next.begin();
    int pre_num = 0;
    int cur_num = number;
    int occ_num = 0;
    int c = -1;
    string s;
    while(it != C_Next.end())
    {
        if (it->second >= cur_num) {
            occ_num = cur_num - pre_num;
            c = it->first;
            s = string(1, (char)c);
            break;
        }
        pre_num = it->second;
        it ++;
    }
    while(1) {
        cur_num = inverse_occ(c, occ_num, row);
        it = C_Next.begin();
        pre_num = 0;
        while(it != C_Next.end())
        {
            if (it->second >= cur_num) {
                occ_num = cur_num - pre_num;
                c = it->first;
                break;
            }
            pre_num = it->second;
            it ++;
        }
        if (c == 91) {
            break;
        }
        if (c == 93 or c == -1) {
            return "";
        }
        s = s + string(1, (char)c);
    }
    return s;
}
