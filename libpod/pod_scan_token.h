#ifndef INCLUDE_POD_SCAN_TOKEN_H
#define INCLUDE_POD_SCAN_TOKEN_H

/******************************************************************** 
 *  pod_scan_token.h
 *  Copyright (c) 2011-2012, James A. Banks
 *  All rights reserved.
 *  See file LICENSE for details.
 ********************************************************************/



typedef enum pod_scan_token {
    POD_TOKEN_STRING,
    POD_TOKEN_EQUALS,
    POD_TOKEN_BEGIN_BLURB,
    POD_TOKEN_END_BLURB,
    POD_TOKEN_BEGIN_LIST,
    POD_TOKEN_END_LIST,
    POD_TOKEN_POD_SYNC
} pod_scan_token;




#endif /* INCLUDE_POD_SCAN_TOKEN_H */
