<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/*">
    <html>
      <head>
        <title><xsl:value-of select="@name"/></title>
      </head>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="QCheckBox">
    <span>
      <xsl:call-template name="style"/>
      <input type="checkbox">
        <xsl:copy-of select="@elementId"/>
        <xsl:if test="@checked = 'true'">
          <xsl:attribute name="checked"/>
        </xsl:if>
      </input>
      <span>
        <xsl:value-of select="@text"/>
      </span>
    </span>
  </xsl:template>
  <xsl:template match="QLabel">
    <span>
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:value-of select="@text"/>
    </span>
  </xsl:template>
  <xsl:template match="QLineEdit">
    <input type="text">
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:attribute name="value">
        <xsl:value-of select="@text"/>
      </xsl:attribute>
      <xsl:if test="@enabled = 'false'">
        <xsl:attribute name="disabled"/>
      </xsl:if>
    </input>
  </xsl:template>
  <xsl:template match="QPushButton|QToolButton">
    <input type="submit">
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:attribute name="value">
        <xsl:value-of select="@text"/>
      </xsl:attribute>
    </input>
  </xsl:template>
  <xsl:template match="QRadioButton">
    <span>
      <xsl:call-template name="style"/>
      <input type="radio">
        <xsl:copy-of select="@elementId"/>
        <xsl:attribute name="name">
          <xsl:text>parent:</xsl:text>
          <xsl:value-of select="../@elementId"/>
        </xsl:attribute>
      </input>
      <span>
        <xsl:value-of select="@text"/>
      </span>
    </span>
  </xsl:template>
  <xsl:template match="QScrollArea">
    <div>
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  <xsl:template match="QTextEdit|QPlainTextEdit">
    <textarea>
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:if test="@enabled = 'false'">
        <xsl:attribute name="disabled"/>
      </xsl:if>
      <xsl:value-of select="@plainText"/>
    </textarea>
  </xsl:template>
  <xsl:template match="QWebView">
    <iframe>
      <xsl:copy-of select="@elementId"/>
      <xsl:call-template name="style"/>
      <xsl:attribute name="src">
        <xsl:value-of select="@url"/>
      </xsl:attribute>
    </iframe>
  </xsl:template>

  <xsl:template name="style">
    <xsl:attribute name="style">
      <xsl:if test="@width">
        <xsl:text>width: </xsl:text><xsl:value-of select="@width"/><xsl:text>;</xsl:text>
      </xsl:if>
      <xsl:if test="@height">
        <xsl:text>height: </xsl:text><xsl:value-of select="@height"/><xsl:text>;</xsl:text>
      </xsl:if>
      <xsl:if test="@visible = 'false'">
        <xsl:text>display: none;</xsl:text>
      </xsl:if>
      <xsl:if test="@styleSheet">
        <xsl:value-of select="@styleSheet"/><xsl:text>;</xsl:text>
      </xsl:if>
    </xsl:attribute>
  </xsl:template>
</xsl:stylesheet>
