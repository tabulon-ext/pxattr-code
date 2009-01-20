#ifndef _pxattr_h_included_
#define _pxattr_h_included_
/* @(#$Id: pxattr.h,v 1.2 2009-01-20 10:50:49 dockes Exp $  (C) 2009 J.F.Dockes */

/**  
 * Provide a uniform C++ API for extended file attributes on Linux/FreeBSD 
 * and MacOSX.
 *
 * We only deal with user attributes. Other domains are even more
 * system-specific. 
 * Linux and FreeBSD handle differently the attributes name space  segmentation.
 * We handle this by using only domain-internal names in the interface:
 * that is, specify the names as 'org.myapp.somename' not
 * 'user.org.myapp.somename', pxattr will deal with adding/removing
 * the 'user.' part as needed. 
 * MacOsX does not segment the attribute name space.
 * 
 * The interface provided should work the same way on all 3 systems,
 * it papers over such differences as the "list" output format,
 * existence of CREATE/UPDATE distinctions, etc.
 * 
 * Diagnostics: all functions return false on error, and preserve the errno 
 * value or set it as appropriate.
 *
 * For path-based interfaces, the PXATTR_NOFOLLOW flag can be set to decide if
 * symbolic links will be acted on or followed.
 */

#include <string>
#include <vector>
using std::string;
using std::vector;

namespace pxattr {

    enum nspace {PXATTR_USER};
    enum flags {PXATTR_NONE=0, PXATTR_NOFOLLOW = 1, 
		PXATTR_CREATE=2, PXATTR_REPLACE=4};

    /**
     * Retrieve the named attribute from path. 
     */
    bool get(const string& path, const string& name, string* value, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * Retrieve the named attribute from open file. 
     */
    bool get(int fd, const string& name, string* value, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * Set the named attribute on path. 
     */
    bool set(const string& path, const string& name, const string& value, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * Set the named attribute on open file. 
     */
    bool set(int fd, const string& name, const string& value, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * Delete the named attribute from path.
     */
    bool del(const string& path, const string& name, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * Delete the named attribute from open file.
     */
    bool del(int fd, const string& name, 
	     flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * List attribute names from path.
     */
    bool list(const string& path, vector<string>* names, 
	      flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);
    /**
     * List attribute names from open file.
     */
    bool list(int fd, vector<string>* names, 
	      flags flags = PXATTR_NONE, nspace dom = PXATTR_USER);

    /**
     * Compute actual/system attribute name from external name 
     * (ie: myattr->user.myattr)
     */
    bool sysname(nspace dom, const string& pname, string* sname);
    /**
     * Compute external name from actual/system name 
     * (ie: user.myattr->myattr)
     */
    bool pxname(nspace dom, const string& sname, string* pname);
}


#endif /* _pxattr_h_included_ */
