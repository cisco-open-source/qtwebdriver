#ifndef WEBDRIVER_QT_WEB_VIEW_EXECUTOR_H_
#define WEBDRIVER_QT_WEB_VIEW_EXECUTOR_H_

#include "extension_qt/q_view_executor.h"

#include <QtCore/QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#include <QtWebKit/QWebHistory>
#else
#include <QtWebKit/QtWebKit>
#endif

namespace webdriver {

class FramePath;
class ValueParser;

class QPageLoader : public QObject {
    Q_OBJECT
public:
    explicit QPageLoader(QWebView* view) :
                QObject(NULL) {webView = view; is_loading = false;}

    void loadPage(QUrl url);
    void reloadPage();
    bool isLoading() {return is_loading;}
    
signals:
    void loaded();
    
public slots:
    void pageLoadStarted();
    void pageLoadFinished();
private:
    bool is_loading;
    QWebView* webView;
    
};

//Notify automation module about end of execution of async script
class JSNotifier : public QObject
{
    Q_OBJECT

public:
    JSNotifier();
    QVariant getResult();
    bool IsCompleted();


public slots:
    void setResult(QVariant result);

signals:
    void completed();

private:
    QVariant res;
    bool isCompleted;
};

class QWebViewCmdExecutorCreator : public ViewCmdExecutorCreator  {
public:
    static const ViewType WEB_VIEW_TYPE; 

    QWebViewCmdExecutorCreator();
    virtual ~QWebViewCmdExecutorCreator();

    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const;
private:

    DISALLOW_COPY_AND_ASSIGN(QWebViewCmdExecutorCreator);
};	

/// QWebView based view's implementation
class QWebViewCmdExecutor : public QViewCmdExecutor {
public:
    explicit QWebViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~QWebViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error);
    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void GoForward(Error** error);
    virtual void GoBack(Error** error);
    virtual void Reload(Error** error);
    virtual void GetSource(std::string* source, Error** error);
    virtual void SendKeys(const ElementId& element, const string16& keys, Error** error);
    virtual void MouseDoubleClick(Error** error);
    virtual void MouseButtonUp(Error** error);
    virtual void MouseButtonDown(Error** error);
    virtual void MouseClick(MouseButton button, Error** error);
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, Error** error);
    virtual void ClickElement(const ElementId& element, Error** error);
    virtual void GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error);
    virtual void ClearElement(const ElementId& element, Error** error);
    virtual void IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error);
    virtual void IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error);
    virtual void ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error);
    virtual void GetElementLocation(const ElementId& element, Point* location, Error** error);
    virtual void GetElementLocationInView(const ElementId& element, Point* location, Error** error);
    virtual void GetElementTagName(const ElementId& element, std::string* tag_name, Error** error);
    virtual void IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error);
    virtual void SetOptionElementSelected(const ElementId& element, bool selected, Error** error);
    virtual void GetElementSize(const ElementId& element, Size* size, Error** error);
    virtual void ElementSubmit(const ElementId& element, Error** error);
    virtual void GetElementText(const ElementId& element, std::string* element_text, Error** error);
    virtual void GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error);
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error);
    virtual void ActiveElement(ElementId* element, Error** error);
    virtual void SwitchTo(Error** error);
    virtual void SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error);
    virtual void SwitchToFrameWithIndex(int index, Error** error);
    virtual void SwitchToFrameWithElement(const ElementId& element, Error** error);
    virtual void SwitchToTopFrame(Error** error);
    virtual void SwitchToTopFrameIfCurrentFrameInvalid(Error** error);
    virtual void NavigateToURL(const std::string& url, bool sync, Error** error);
    virtual void GetURL(std::string* url, Error** error);
    virtual void ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void GetAppCacheStatus(int* status, Error** error);
    virtual void GetCookies(const std::string& url, base::ListValue** cookies, Error** error);
    virtual void SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error);
    virtual void DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error);
    virtual void GetStorageKeys(StorageType type, base::ListValue** keys, Error** error);
    virtual void SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error);
    virtual void ClearStorage(StorageType type, Error** error);
    virtual void GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error);
    virtual void RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error);
    virtual void GetStorageSize(StorageType type, int* size, Error** error);
    virtual void GetGeoLocation(base::DictionaryValue** geolocation, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetGeoLocation(const base::DictionaryValue* geolocation, Error** error) NOT_SUPPORTED_IMPL;
    virtual void TouchClick(const ElementId& element, Error **error);
    virtual void TouchDoubleClick(const ElementId& element, Error **error);
    virtual void TouchDown(const int &x, const int &y, Error **error);
    virtual void TouchUp(const int &x, const int &y, Error **error);
    virtual void TouchMove(const int &x, const int &y, Error **error);
    virtual void TouchLongClick(const ElementId& element, Error **error);
    virtual void TouchScroll(const int &xoffset, const int &yoffset, Error **error);
    virtual void TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error);
    virtual void TouchFlick(const int &xSpeed, const int &ySpeed, Error **error);
    virtual void TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error);


protected:
	QWebView* getView(const ViewId& viewId, Error** error);
	QWebFrame* FindFrameByPath(QWebFrame* parent, const FramePath &frame_path);
	QWebFrame* GetFrame(QWebView* view, const FramePath& frame_path);
	Error* ExecuteScriptAndParse(QWebFrame* frame,
                                const std::string& anonymous_func_script,
                                const std::string& script_name,
                                const base::ListValue* args,
                                const ValueParser* parser);
	Error* ExecuteAsyncScript(QWebFrame* frame,
                            const std::string& script,
                            const base::ListValue* const args,
                            base::Value** value);
	Error* ExecuteScript(QWebFrame* frame,
                        const std::string& script,
                        const base::ListValue* const args,
                        base::Value** value);
	Error* ExecuteScriptAndParseValue(QWebFrame* frame,
                                    const std::string& script,
                                    base::Value** script_result, bool isAsync);
	Error* ExecuteScriptImpl(QWebFrame* frame,
                               const std::string &script,
                               std::string *result,
                               bool isAsync);

	Error* FindElementsHelper(QWebFrame* frame,
                            const ElementId& root_element,
                            const std::string& locator,
                            const std::string& query,
                            bool find_one,
                            std::vector<ElementId>* elements);

	Error* ExecuteFindElementScriptAndParse(
    			QWebFrame* frame,
    			const ElementId& root_element,
    			const std::string& locator,
    			const std::string& query,
    			bool find_one,
    			std::vector<ElementId>* elements);

	Error* GetElementRegionInViewHelper(
		    	QWebFrame* frame,
		    	const ElementId& element,
		    	const Rect& region,
		    	bool center,
		    	bool verify_clickable_at_middle,
		    	Point* location);

	Error* GetElementRegionInView(
				QWebView* view,
		    	const ElementId& element,
		    	const Rect& region,
		    	bool center,
		    	bool verify_clickable_at_middle,
		    	Point* location);

	Error* VerifyElementIsClickable(
			    QWebFrame* frame,
			    const ElementId& element,
			    const Point& location);

	Error* GetElementBorder(QWebFrame* frame,
                                const ElementId& element,
                                int* border_left,
                                int* border_top);

	Error* GetElementEffectiveStyle(
		    	QWebFrame* frame,
		    	const ElementId& element,
		    	const std::string& prop,
		    	std::string* value);

	QWebFrame* FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path);

	void AddIdToCurrentFrame(QWebView* view, const FramePath &frame_path);

	Error* SwitchToFrameWithJavaScriptLocatedFrame(
								QWebView* view,
								QWebFrame* frame,
    							const std::string& script,
    							base::ListValue* args);

	Error* GetElementFirstClientRect(QWebFrame* frame,
                                    const ElementId& element,
                                    Rect* rect);

	Error* GetClickableLocation(QWebView* view, const ElementId& element, Point* location);

	Error* ToggleOptionElement(const ElementId& element);




    
private:
    DISALLOW_COPY_AND_ASSIGN(QWebViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_EXECUTOR_H_
