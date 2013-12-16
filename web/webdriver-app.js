// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/startsWith
if (!String.prototype.startsWith) {
  Object.defineProperty(String.prototype, 'startsWith', {
    enumerable: false,
    configurable: false,
    writable: false,
    value: function (searchString, position) {
      position = position || 0;
      return this.indexOf(searchString, position) === position;
    }
  });
}

if (!String.prototype.endsWith) {
  Object.defineProperty(String.prototype, 'endsWith', {
    enumerable: false,
    configurable: false,
    writable: false,
    value: function (searchString, position) {
      position = position || this.length;
      position = position - searchString.length;
      var lastIndex = this.lastIndexOf(searchString);
      return lastIndex !== -1 && lastIndex === position;
    }
  });
}

webdriver.ExistingSessionBuilder = function() {
  webdriver.AbstractBuilder.call(this);
};

webdriver.ExistingSessionBuilder.prototype = Object.create(webdriver.AbstractBuilder.prototype);

webdriver.ExistingSessionBuilder.prototype.build = function() {
  var self = this;
  var client = new webdriver.http.CorsClient(this.getServerUrl());
  var executor = new webdriver.http.Executor(client);
  var session = webdriver.Session.getSessions(executor).then(function(sessions) {
    if (sessions[0])
      return sessions[0];
    else
      return webdriver.WebDriver.createSession(executor, self.getCapabilities()).getSession();
  });
  return new webdriver.WebDriver(session, executor);
};

webdriver.WebDriver.prototype.visualizerGetSource = function() {
  webdriver.http.Executor.COMMAND_MAP_['visualizerGetSource'] = {
    method: 'GET', path: '/session/:sessionId/-cisco-visualizer-source'};
  return this.schedule(
    new webdriver.Command('visualizerGetSource'),
    'WebDriver.visualizerGetSource()');
};

webdriver.WebDriver.prototype.visualizerShowPoint = function() {
  webdriver.http.Executor.COMMAND_MAP_['visualizerShowPoint'] = {
    method: 'GET', path: '/session/:sessionId/-cisco-visualizer-show-point'};
  return this.schedule(
    new webdriver.Command('visualizerShowPoint'),
    'WebDriver.visualizerShowPoint()');
};

function WebDriverProxy() {
}

WebDriverProxy.prototype.setServerUrl = function(serverUrl) {
  var self = this;
  this.driver_ = new webdriver.ExistingSessionBuilder().
    usingServer(serverUrl).
    withCapabilities({'browserName': 'qtwebkit'}).
    build();

  for (var propertyName in this.driver_) {
    var property = this.driver_[propertyName];
    if (typeof property === 'function') {
      this[propertyName] = function(impl) {
        return function() { 
          return impl.apply(self.driver_, arguments);
        };
      }(property);
    }
  }
};

WebDriverProxy.prototype.quit = function() {
};

window.onerror = function(errorMsg, url, lineNumber, columnNumber, error) {
  console.log(error.stack);
  return false;
};

var Util = {};

Util.loadFile = function(url) {
  var req = new XMLHttpRequest();
  req.open('GET', url, false);
  try {
    req.send();
    return req.responseText;
  } catch (err) {
    var message = 'Error at "' + err.filename + '" message "' + err.message +
      '" name "' + err.name + '" result 0x' + Number(err.result).toString(16);
    console.log(message);
    return message;
  }
};

Util.getXPath = function() {
  function getXPath(node, path) {
    path = path || [];
    if (node.parentNode) {
      path = getXPath(node.parentNode, path);
    }

    if (node.previousSibling) {
      var count = 1;
      var sibling = node.previousSibling
      do {
        if (sibling.nodeType == 1 && sibling.nodeName == node.nodeName) {count++;}
        sibling = sibling.previousSibling;
      } while (sibling);
      if (count == 1) {count = null;}
    } else if (node.nextSibling) {
      var sibling = node.nextSibling;
      do {
        if (sibling.nodeType == 1 && sibling.nodeName == node.nodeName) {
          var count = 1;
          sibling = null;
        } else {
          var count = null;
          sibling = sibling.previousSibling;
        }
      } while (sibling);
    }

    if (node.nodeType == 1) {
      path.push(node.nodeName.toLowerCase() + (node.id ? "[@id='"+node.id+"']" : count > 0 ? "["+count+"]" : ''));
    }
    return path;
  }

  return function(node) {
    if (node.hasAttribute('data-viz-id'))
      return '//*[@data-viz-id=' + node.getAttribute('data-viz-id') + "]";

    var path = getXPath(node);
    return '/' + path.join('/');
  }
}();

Util.webDriverKeyFromJs = function(keyCode) {
  switch (keyCode) {
    case  8: return webdriver.Key.BACK_SPACE;
    case  9: return webdriver.Key.TAB;
    case 13: return webdriver.Key.ENTER;
    case 16: return webdriver.Key.SHIFT;
    case 17: return webdriver.Key.CONTROL;
    case 18: return webdriver.Key.ALT;
    case 19: return webdriver.Key.PAUSE;
    //case 20: return ; //  caps lock
    case 27: return webdriver.Key.ESCAPE;
    case 33: return webdriver.Key.PAGE_UP;
    case 34: return webdriver.Key.PAGE_DOWN;
    case 35: return webdriver.Key.END;
    case 36: return webdriver.Key.HOME;
    case 37: return webdriver.Key.ARROW_LEFT;
    case 38: return webdriver.Key.ARROW_UP;
    case 39: return webdriver.Key.ARROW_RIGHT;
    case 40: return webdriver.Key.ARROW_DOWN;
    case 45: return webdriver.Key.INSERT;
    case 46: return webdriver.Key.DELETE;
    //case 91: return ; // left window
    //case 92: return ; // right window
    //case 93: return ; // select key
    case 96: return webdriver.Key.NUMPAD0;
    case 97: return webdriver.Key.NUMPAD1;
    case 98: return webdriver.Key.NUMPAD2;
    case 99: return webdriver.Key.NUMPAD3;
    case 100: return webdriver.Key.NUMPAD4;
    case 101: return webdriver.Key.NUMPAD5;
    case 102: return webdriver.Key.NUMPAD6;
    case 103: return webdriver.Key.NUMPAD7;
    case 104: return webdriver.Key.NUMPAD8;
    case 105: return webdriver.Key.NUMPAD9;
    case 106: return webdriver.Key.MULTIPLY;
    case 107: return webdriver.Key.ADD;
    case 109: return webdriver.Key.SUBTRACT;
    case 110: return webdriver.Key.DECIMAL; // decimal point
    case 111: return webdriver.Key.DIVIDE;
    case 112: return webdriver.Key.F1;
    case 113: return webdriver.Key.F2;
    case 114: return webdriver.Key.F3;
    case 115: return webdriver.Key.F4;
    case 116: return webdriver.Key.F5;
    case 117: return webdriver.Key.F6;
    case 118: return webdriver.Key.F7;
    case 119: return webdriver.Key.F8;
    case 120: return webdriver.Key.F9;
    case 121: return webdriver.Key.F10;
    case 122: return webdriver.Key.F11;
    case 123: return webdriver.Key.F12;
    //case 144: return ; // num lock
    //case 145: return ; // scroll lock
    case 186: return webdriver.Key.SEMICOLON;
    case 187: return webdriver.Key.EQUALS;
    //case 188: return ; // comma
    //case 189: return ; // dash
    //case 190: return .; // period
    //case 191: return /; // forward slash
    //case 192: return `; // grave accent
    //case 219: return [; // open bracket
    //case 220: return \\; // back slash
    //case 221: return ]; // close bracket
    //case 222: return '; // single quote
  }
};

function VisualizerXsltProcessors() {
  this.widget = this.create_('widget_view_visualizer.xsl');
  this.qml = this.create_('qml_view_visualizer.xsl');
}

VisualizerXsltProcessors.prototype.get = function(webPage) {
  if (webPage.startsWith('qtwidget://'))
    return this.widget;
  if (webPage.endsWith('.qml'))
    return this.qml;
};

VisualizerXsltProcessors.prototype.create_ = function(name) {
  var stylesheet = Util.loadFile(name);
  stylesheet = (new DOMParser()).parseFromString(stylesheet, 'application/xml');
  var processor = new XSLTProcessor();
  try {
    processor.importStylesheet(stylesheet);
    return processor;
  } catch (err) {
    console.log(err);
  }
};

function VisualizerController(driver) {
  this.driver = driver;
  this.xsltProcessors = new VisualizerXsltProcessors();
}

VisualizerController.prototype.updateSource = function(webPage) {
  var self = this;
  this.webPage = webPage;
  this.driver.visualizerGetSource().then(function(source) {
    self.driver.manage().window().getSize().then(function(targetSize) {
      self.showVisualizationWindow(source, targetSize);
    });
  });
};

VisualizerController.prototype.quit = function() {
  this.webPage = null;
  if (this.visualizationWin) {
    this.visualizationWin.close();
    this.visualizationWin = null;
  }
};

VisualizerController.prototype.isVisualizerOpened = function() {
  return this.visualizationWin && !this.visualizationWin.closed && this.visualizationWin.innerHeight > 0;
};

VisualizerController.prototype.openVisualizationWindow = function(size) {
  var winParams = 'width=' + size.width + ',height=' + size.height;
  this.visualizationWin = window.open('', '', winParams);
};

VisualizerController.prototype.visualizerAssignEventHandlers = function() {
  var self = this;
  var win = this.visualizationWin;

  win.document.onkeypress = function(event) {
    var key = String.fromCharCode(event.charCode);

    if (event.target.hasAttribute('elementId')) {
      var elementId = event.target.getAttribute('elementId');
      var element = new webdriver.WebElement(self.driver, elementId);
      element.sendKeys(key);
    } else {
      self.driver.actions().sendKeys(key).perform();
    }

    return true;
  };

  win.document.onkeyup = function(event) {
    var key = Util.webDriverKeyFromJs(event.keyCode);
    if (!key)
      return true;

    if (event.target.hasAttribute('elementId')) {
      var elementId = event.target.getAttribute('elementId');
      var element = new webdriver.WebElement(self.driver, elementId);
      element.sendKeys(key);
    } else {
      self.driver.actions().sendKeys(key).perform();
    }

    return true;
  };

  win.document.onmousedown = function(event) {
    var disableMouseEvents = document.getElementsByName('disableMouseEvents')[0].checked;
    if (disableMouseEvents) {
      return;
    }

    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      mouseMove(target, {x: event.offsetX, y: event.offsetY}).
      mouseDown(event.button).
      perform();
  };

  win.document.onmouseup = function(event) {
    var disableMouseEvents = document.getElementsByName('disableMouseEvents')[0].checked;
    if (disableMouseEvents) {
      return;
    }

    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      mouseMove(target, {x: event.offsetX, y: event.offsetY}).
      mouseUp(event.button).
      perform();
  };

  win.document.onclick = function(event) {
    var disableMouseEvents = document.getElementsByName('disableMouseEvents')[0].checked;
    if (disableMouseEvents) {
      return false;
    }

    if (event.target.hasAttribute('elementId')) {
      var elementId = event.target.getAttribute('elementId');
      var element = new webdriver.WebElement(self.driver, elementId);
      element.click();
    } else {
      var xpath = Util.getXPath(event.target);
      var target = self.driver.findElement(webdriver.By.xpath(xpath));
      self.driver.actions().
        mouseMove(target, {x: event.offsetX, y: event.offsetY}).
        click(event.button).
        perform().
      then(function() {
        return self.driver.visualizerShowPoint();
      });
    }

    return false;
  };

  Hammer.detection.gestures = [];
  Hammer.READY = false;
  Hammer.DOCUMENT = win.document;
  var hammer = Hammer(win.document);

  hammer.on("tap", function(event) {
    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      touchSingleTap(target).
      perform();
  });

  hammer.on("hold", function(event) {
    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      touchLongPress(target).
      perform();
  });

  hammer.on("doubletap", function(event) {
    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      touchDoubleTap(target).
      perform();
  });

  hammer.on("touch", function(event) {
    self.driver.actions().
      touchDown(event.gesture.center.pageX, event.gesture.center.pageY).
      perform();
  });

  hammer.on("release", function(event) {
    self.driver.actions().
      touchUp(event.gesture.center.pageX, event.gesture.center.pageY).
      perform();
  });

  hammer.on("drag", function(event) {
    self.driver.actions().
      touchMove(event.gesture.center.pageX, event.gesture.center.pageY).
      perform();
  });

  hammer.on("dragstart", function(event) {
    var xpath = Util.getXPath(event.target);
    self.dragstart = self.driver.findElement(webdriver.By.xpath(xpath));
  });

  hammer.on("dragend", function(event) {
    if (self.dragstart) {
      self.driver.actions().
        touchScroll(self.dragstart, {x: event.gesture.deltaX, y: event.gesture.deltaY}).
        perform();
    } else {
      self.driver.actions().
        touchScroll({x: event.gesture.deltaX, y: event.gesture.deltaY}).
        perform();
    }
    self.dragstart = null;
  });

  hammer.on("swipe", function(event) {
    if (self.dragstart) {
      var offset = {x: event.gesture.deltaX, y: event.gesture.deltaY};
      var speed = Math.sqrt(offset.x * offset.x + offset.y * offset.y) * 1000 / event.gesture.deltaTime;
      self.driver.actions().
        touchFlick(self.dragstart, offset, speed).
        perform();
    } else {
      var xspeed = Math.floor(event.gesture.velocityX * 1000);
      var yspeed = Math.floor(event.gesture.velocityY * 1000);
      self.driver.actions().
        touchFlick({x: xspeed, y: yspeed}).
        perform();
    }
  });

  hammer.on("rotate", function(event) {
    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      touchPinchRotate(target, event.gesture.rotation).
      perform();
  });

  hammer.on("pinch", function(event) {
    var xpath = Util.getXPath(event.target);
    var target = self.driver.findElement(webdriver.By.xpath(xpath));
    self.driver.actions().
      touchPinchZoom(target, event.gesture.scale).
      perform();
  });
};

VisualizerController.prototype.showVisualizationWindow = function(source, size) {
  var isQt = this.webPage.startsWith('qtwidget://') || this.webPage.endsWith('.qml');
  var isWinOpened = this.isVisualizerOpened();

  if (!isQt && isWinOpened) {
    this.visualizationWin.document.documentElement.innerHTML = source;
    this.visualizationWin.resizeTo(size.width, size.height);
    return;
  }

  if (!isWinOpened)
    this.openVisualizationWindow(size);

  if (isQt) {
    var xsltProcessor = this.xsltProcessors.get(this.webPage);
    var receivedDoc = (new DOMParser()).parseFromString(source, 'application/xml');
    receivedDoc = xsltProcessor.transformToDocument(receivedDoc);
    var visualizerDoc = this.visualizationWin.document;
    visualizerDoc.replaceChild(receivedDoc.documentElement, visualizerDoc.documentElement);
  } else {
    var visualizerDoc = this.visualizationWin.document;
    visualizerDoc.write(source);
  }

  this.visualizerAssignEventHandlers();
};

function WebDriverJsView() {
  if (localStorage && localStorage.webDriverUrlPort) {
    this.setDriverUrlPort(localStorage.webDriverUrlPort);
  } else {
    this.setDriverUrlPort('');
  }

  if (localStorage && localStorage.webPage) {
    this.setWebPage(localStorage.webPage);
  } else {
    this.setWebPage('');
  }
}

WebDriverJsView.prototype.getDriverUrlPort = function() {
  var input = document.getElementsByName('webDriverUrlPort')[0];
  return input.value;
};

WebDriverJsView.prototype.setDriverUrlPort = function(value) {
  var input = document.getElementsByName('webDriverUrlPort')[0];
  input.value = value;
  this.updateSessionDepControls();
};

WebDriverJsView.prototype.getWebPage = function() {
  var input = document.getElementsByName('webPage')[0];
  return input.value;
};

WebDriverJsView.prototype.setWebPage = function(value) {
  var input = document.getElementsByName('webPage')[0];
  input.value = value;
  this.updateSessionDepControls();
};

WebDriverJsView.prototype.updateSessionDepControls = function() {
  var disable = this.getDriverUrlPort().trim() === '' ||
                this.getWebPage().trim() === '';

  var sessionDepControls = [];
  sessionDepControls.push(document.getElementById('sourceButton'));
  sessionDepControls.push(document.getElementById('screenshotButton'));
  for (var controlIndex in sessionDepControls) {
    var control = sessionDepControls[controlIndex];
    control.disabled = disable;
  }
}

function WebDriverJsController() {
  this.driver = new WebDriverProxy();
  this.visualizer = new VisualizerController(this.driver);
  this.view = new WebDriverJsView();
}

WebDriverJsController.prototype.setServerUrl = function(serverUrl) {
  this.driver.setServerUrl(serverUrl);

  this.serverUrl = serverUrl;
  if (localStorage)
    localStorage.serverUrl = serverUrl;
}

WebDriverJsController.prototype.setWebPage = function(webPage) {
  this.element = null;

  this.driver.get(webPage);

  this.webPage = webPage;
  if (localStorage)
    localStorage.webPage = webPage;
};

WebDriverJsController.prototype.updateDriver = function() {
  var webDriverUrlPort = document.getElementsByName('webDriverUrlPort')[0].value;
  if (webDriverUrlPort !== this.webDriverUrlPort)
    this.setServerUrl(webDriverUrlPort);

  var webPage = document.getElementsByName('webPage')[0].value;
  if (webPage !== this.webPage)
    this.setWebPage(webPage);
};

WebDriverJsController.prototype.onGet = function() {
  this.webPage = null;
  this.updateDriver();
};

WebDriverJsController.prototype.onSource = function() {
  this.updateDriver();
  this.visualizer.updateSource(this.webPage);
};

WebDriverJsController.prototype.onScreenshot = function() {
  this.updateDriver();
  this.driver.takeScreenshot().then(function(data) {
    data = base64_arraybuffer.decode(data);
    data = new Blob([data], {type: 'image/png'});
    saveAs(data, 'screenshot.png');
  })
};

WebDriverJsController.prototype.onLogs = function(type) {
  if (type === 'Logs')
    return;

  this.updateDriver();
  this.driver.manage().logs().get(type).then(function(entries) {
    var lines = [];
    for (var entryIndex in entries) {
      var entry = entries[entryIndex];
      lines.push(entry.level.name + ' ' + entry.timestamp + ' ' + entry.message);
    }

    data = new Blob([lines.join('\n')], {type: 'text/plain'});
    saveAs(data, 'webdriver.log');
  })
};

WebDriverJsController.prototype.onFindElement = function() {
  var criteria = document.getElementsByName('findElementCriteria')[0].value;
  var key = document.getElementsByName('findElementKey')[0].value;
  if (criteria === 'id')
    this.element = this.driver.findElement(webdriver.By.id(key));
  else if (criteria === 'name')
    this.element = this.driver.findElement(webdriver.By.name(key));
  else if (criteria === 'tagName')
    this.element = this.driver.findElement(webdriver.By.tagName(key));
  else if (criteria === 'xpath')
    this.element = this.driver.findElement(webdriver.By.xpath(key));
};

WebDriverJsController.prototype.onSendKeys = function(key) {
  if (this.element) {
    this.element.sendKeys(key);
  } else {
    this.driver.actions().sendKeys(key).perform();
  }
};

WebDriverJsController.prototype.onClick = function() {
  this.element.click();
};

WebDriverJsController.prototype.onListWindowHandles = function() {
  var select = document.getElementById('windowList');
  this.driver.getAllWindowHandles().then(function(handles) {
    select.innerHTML = '';
    for (var handleIndex in handles) {
      var handle = handles[handleIndex];
      var item = document.createElement('option');
      item.setAttribute('value', handle);
      item.innerHTML = handle;
      select.appendChild(item)
    }
    document.getElementById('windowList').style.visibility = 'visible';
    document.getElementById('chooseWindow').style.visibility = 'visible';
  });
};

WebDriverJsController.prototype.onChooseWindow = function() {
  var handle = document.getElementById('windowList').value;
  this.driver.switchTo().window(handle);
};

WebDriverJsController.prototype.onSetWindowSize = function() {
  var width = document.getElementsByName('windowSizeWidth')[0].value;
  var height = document.getElementsByName('windowSizeHeight')[0].value;
  width = parseInt(width);
  height = parseInt(height);
  this.driver.manage().window().setSize(width, height);
};

WebDriverJsController.prototype.onQuit = function() {
  this.driver.quit();
  this.visualizer.quit();
  this.webDriverUrlPort = null;
  this.webPage = null;
};

WebDriverJsController.prototype.onWebDriverUrlPortChange = function() {
  this.view.updateSessionDepControls();
};

WebDriverJsController.prototype.onWebPageChange = function() {
  this.view.updateSessionDepControls();
};

document.addEventListener("DOMContentLoaded", function(event) {
  window.wd = new WebDriverJsController();
});
