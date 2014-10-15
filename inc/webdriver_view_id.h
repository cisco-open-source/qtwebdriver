/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

/*! \page page_views Views management and actions

<h1>View handles</h1>

Session contains map of ViewId to ViewHandle. webdriver::ViewHandle is abstraction
intended to keep reference to real window implementation(ie pointer to QWidget).
Customizer should implement this abstract class. 

Example of custom implementation:
\code
class QViewHandle : public ViewHandle {
public:
    QViewHandle();
    QViewHandle(QWidget* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QWidget* get() { return view_.data(); };
    
protected:
    QPointer<QWidget> view_;
    virtual ~QViewHandle() {};
};
\endcode

*/

#ifndef WEBDRIVER_WEBDRIVER_VIEW_ID_H_
#define WEBDRIVER_WEBDRIVER_VIEW_ID_H_

#include <string>

#include "base/memory/ref_counted.h"    

namespace webdriver {

typedef unsigned long int ViewType;

/// This class represents a Webdriver View handle.
/// It keeps implementation's dependent view pointer or object.
/// Customizer should inherit this class and implement.
class ViewHandle : public base::RefCounted<ViewHandle>{
public:
    ViewHandle() {};

    /// @return true if handle references valid view
    virtual bool is_valid() const = 0;
    /// compares two handle
    /// @param other handle to compare
    /// @return true if two handles reference same view
    virtual bool equals(const ViewHandle* other) const = 0;
protected:
    friend class base::RefCounted<ViewHandle>;
    virtual ~ViewHandle() {};
};

typedef scoped_refptr<ViewHandle> ViewHandlePtr;

/// This class represents a Webdriver View ID. These IDs are mapped to objects in a page.
class ViewId {
public:
    /// Creates an invalid ID.
    ViewId();

    /// Creates a valid ViewId using the ID of an view.
    /// @param id view Id
    explicit ViewId(const std::string& id);

    ~ViewId();

    /// Returns whether this ID is valid.
    bool is_valid() const;

    /// Returns string form of id
    const std::string& id() const { return id_; }

    bool operator==(const ViewId &other) const;

    bool operator!=(const ViewId &other) const;

    bool operator<(const ViewId &other) const;

private:
    std::string id_;
    bool is_valid_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_VIEW_ID_H_
