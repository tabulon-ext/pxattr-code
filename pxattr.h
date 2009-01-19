#ifndef _pxattr_h_included_
#define _pxattr_h_included_
/* @(#$Id: pxattr.h,v 1.1 2009-01-19 17:02:12 dockes Exp $  (C) 2009 J.F.Dockes */

/**  
 * Provide a uniform C++ API for extended file attributes on Linux/FreeBSD 
 * and MacOSX
 */

#include <sys/types.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

namespace pxattr {

    enum nspace {PXATTR_USER};
    bool get(const string& path, nspace dom, const string& name, 
		string* value);
    bool set(const string& path, nspace dom, const string& name, 
		const string& value);
    bool del(const string& path, nspace dom, const string& name);
    bool list(const string& path, nspace dom, vector<string>* names);

    bool sysname(nspace dom, const string& pname, string* sname);
    bool pxname(nspace dom, const string& sname, string* pname);
}


#endif /* _pxattr_h_included_ */
