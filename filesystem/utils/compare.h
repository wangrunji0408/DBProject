/*
 * parser.h
 *
 *  Created on: 2014年12月7日
 *      Author: lql
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "../bplustree/BPlusTree.h"
#include "pagedef.h"
#include <cstring>
using namespace std;
#define LL_TYPE 0
#define DB_TYPE 1
#define ST_TYPE 2
#define L 0
#define G 3
#define LE 1
#define GE 2
#define E 4
#define IS 5
#define UNI 0
#define UNUNI 1
#define N 0
#define UN 1
#define ALL 0
#define RANGE 1
#define ISNULL 2
#define NOTHING 3
int tmp = 0;
template<class Key>
int keyu(uchar* a, uchar* b) {
	Key c, d;
	memcpy(&c, a, sizeof(Key));
	memcpy(&d, b, sizeof(Key));
	return ((c < d) ? -1 : ((c > d) ? 1 : 0));
}
template<class Key>
int keyn(uchar* a, uchar* b) {
    int res = keyu<Key>(a, b);
    return (res != 0) ? res : keyu<ll>(a + sizeof(Key), b + sizeof(Key));
}
int su(uchar* a, uchar* b) {
	int res = strcmp((char*)a, (char*)b);
	return (res > 0) ? 1 : ((res < 0) ? -1 : 0);
}
int sn(uchar* a, uchar* b) {
	int res = strcmp((char*)a, (char*)b);
	res = (res > 0) ? 1 : ((res < 0) ? -1 : 0);
	return (res != 0) ? res : keyu<ll>(a + tmp, b + tmp);
}
cf* kcmp[3][2] = {
	{&keyu<ll>, &keyn<ll>},
	{&keyu<db>, &keyn<db>},
	{&su, &sn}
};
#endif /* PARSER_H_ */
