#ifndef WEBDRIVER_QT_WEB_VIEW_EXECUTOR_H_
#define WEBDRIVER_QT_WEB_VIEW_EXECUTOR_H_

#include "extension_qt/q_view_executor.h"

//#include <QtCore/QObject>
#include <QtWebKit/QtWebKit>

namespace webdriver {

class FramePath;	
class ValueParser;

class QPageLoader : public QObject {
    Q_OBJECT
public:
    explicit QPageLoader(QWebView* view) :
                QObject(NULL) {webView = view; is_loading = false;};

    void loadPage(QUrl url);
    bool isLoading() {return is_loading;};
    
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
    QWebViewCmdExecutorCreator();
    virtual ~QWebViewCmdExecutorCreator(){}

    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const;
private:

    DISALLOW_COPY_AND_ASSIGN(QWebViewCmdExecutorCreator);
};	

/// QWebView based view's implementation
class QWebViewCmdExecutor : public QViewCmdExecutor {
public:
    explicit QWebViewCmdExecutor(Session* session, ViewId viewId);
    ~QWebViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error);
    virtual void GetTitle(std::string* title, Error **error);
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
    /// move mouse to position: cur_point + offset
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error);
    /// Move the mouse by an offset of the specificed element
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error);
    /// the mouse will be moved to the center of the element
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
    virtual void NavigateToURL(const std::string& url, bool sync, Error** error);
    virtual void GetURL(std::string* url, Error** error);
    virtual void ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void GetAppCacheStatus(int* status, Error** error);

protected:
	Error* checkView(const ViewId& viewId, QWebView** webView);
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
