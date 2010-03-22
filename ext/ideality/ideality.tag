<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="class">
    <name>DButtonBar</name>
    <filename>a00014.html</filename>
    <member kind="slot">
      <type>void</type>
      <name>onlyShow</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>i0</anchor>
      <arglist>(DToolView *tool, bool ensureVisible=false)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setExclusive</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>i1</anchor>
      <arglist>(bool excl)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setAutoHide</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>i2</anchor>
      <arglist>(bool autohide)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setShowOnlyIcons</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>i3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setShowOnlyTexts</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>i4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DButtonBar</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(Qt::ToolBarArea area, QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DButtonBar</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addButton</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(DViewButton *viewButton)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeButton</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(DViewButton *viewButton)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isEmpty</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a4</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disable</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a5</anchor>
      <arglist>(DViewButton *v)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enable</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a6</anchor>
      <arglist>(DViewButton *v)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isExclusive</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a7</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>autohide</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a8</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>showSeparator</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a9</anchor>
      <arglist>(bool e)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>count</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a10</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setEnableButtonBlending</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>a11</anchor>
      <arglist>(bool enable)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mousePressEvent</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>enterEvent</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>b1</anchor>
      <arglist>(QEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>leaveEvent</name>
      <anchorfile>a00014.html</anchorfile>
      <anchor>b2</anchor>
      <arglist>(QEvent *e)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DefaultSettings</name>
    <filename>a00015.html</filename>
    <base>DMainWindowAbstractSettings</base>
    <member kind="function">
      <type></type>
      <name>DefaultSettings</name>
      <anchorfile>a00015.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QObject *parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DefaultSettings</name>
      <anchorfile>a00015.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>a00015.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(DMainWindow *w)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>restore</name>
      <anchorfile>a00015.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(DMainWindow *w)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DMainWindow</name>
    <filename>a00016.html</filename>
    <member kind="slot">
      <type>void</type>
      <name>setEnableButtonBlending</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>i0</anchor>
      <arglist>(bool enable)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DMainWindow</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DMainWindow</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DToolView *</type>
      <name>addToolView</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(QWidget *widget, Qt::DockWidgetArea area, int perspective=DefaultPerspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>moveToolView</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(DToolView *view, Qt::DockWidgetArea newPlace)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addToPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a4</anchor>
      <arglist>(QWidget *widget, int perspective=DefaultPerspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeFromPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a5</anchor>
      <arglist>(QWidget *widget)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addToPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a6</anchor>
      <arglist>(QAction *action, int perspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addToPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a7</anchor>
      <arglist>(const QList&lt; QAction * &gt; &amp;actions, int perspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeFromPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a8</anchor>
      <arglist>(QAction *action)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCurrentPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a9</anchor>
      <arglist>(int wsp)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>currentPerspective</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a10</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setAutoRestore</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a11</anchor>
      <arglist>(bool autoRestore)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>autoRestore</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a12</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual QMenu *</type>
      <name>createPopupMenu</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a13</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSettingsHandler</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a14</anchor>
      <arglist>(DMainWindowAbstractSettings *settings)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>restoreGUI</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a15</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveGUI</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a16</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QHash&lt; Qt::ToolBarArea, DButtonBar * &gt;</type>
      <name>buttonBars</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a17</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QHash&lt; DButtonBar *, QList&lt; DToolView * &gt; &gt;</type>
      <name>toolViews</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>a18</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>addButtonBar</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(Qt::ToolBarArea area)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>closeEvent</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>b1</anchor>
      <arglist>(QCloseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>showEvent</name>
      <anchorfile>a00016.html</anchorfile>
      <anchor>b2</anchor>
      <arglist>(QShowEvent *e)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DMainWindowAbstractSettings</name>
    <filename>a00017.html</filename>
    <member kind="function">
      <type></type>
      <name>DMainWindowAbstractSettings</name>
      <anchorfile>a00017.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QObject *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DMainWindowAbstractSettings</name>
      <anchorfile>a00017.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DMainWindowFactory</name>
    <filename>a00018.html</filename>
    <member kind="function">
      <type></type>
      <name>DMainWindowFactory</name>
      <anchorfile>a00018.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DMainWindowFactory</name>
      <anchorfile>a00018.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DMainWindow *</type>
      <name>create</name>
      <anchorfile>a00018.html</anchorfile>
      <anchor>e0</anchor>
      <arglist>(QMainWindow *other)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DStackedMainWindow</name>
    <filename>a00019.html</filename>
    <base>DMainWindow</base>
    <member kind="function">
      <type></type>
      <name>DStackedMainWindow</name>
      <anchorfile>a00019.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DStackedMainWindow</name>
      <anchorfile>a00019.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWidget</name>
      <anchorfile>a00019.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(QWidget *widget, int perspective=DefaultPerspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeWidget</name>
      <anchorfile>a00019.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(QWidget *widget)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DTabbedMainWindow</name>
    <filename>a00020.html</filename>
    <base>DMainWindow</base>
    <member kind="function">
      <type></type>
      <name>DTabbedMainWindow</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DTabbedMainWindow</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWidget</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(QWidget *widget, bool persistant=false, int perspective=DefaultPerspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeWidget</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(QWidget *widget)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTabWidget</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a4</anchor>
      <arglist>(QTabWidget *w)</arglist>
    </member>
    <member kind="function">
      <type>QTabWidget *</type>
      <name>tabWidget</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>a5</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="slot" protection="protected">
      <type>void</type>
      <name>closeCurrentTab</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>j0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>setupPerspective</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>j1</anchor>
      <arglist>(int wps)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>setupTabWidget</name>
      <anchorfile>a00020.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QTabWidget *w)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DToolView</name>
    <filename>a00021.html</filename>
    <member kind="function">
      <type></type>
      <name>DToolView</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(const QString &amp;title, const QIcon &amp;icon=QIcon(), QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DToolView</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDescription</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(const QString &amp;description)</arglist>
    </member>
    <member kind="function">
      <type>DViewButton *</type>
      <name>button</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>sizeHint</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a4</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPerspective</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a5</anchor>
      <arglist>(int wsp)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>perspective</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a6</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFixedSize</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a7</anchor>
      <arglist>(int s)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fixedSize</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>a8</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>showEvent</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QShowEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual bool</type>
      <name>event</name>
      <anchorfile>a00021.html</anchorfile>
      <anchor>b1</anchor>
      <arglist>(QEvent *e)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DViewButton</name>
    <filename>a00022.html</filename>
    <member kind="slot">
      <type>void</type>
      <name>setSensible</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>i0</anchor>
      <arglist>(bool s)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setBlending</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>i1</anchor>
      <arglist>(bool e)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setOnlyText</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>i2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>setOnlyIcon</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>i3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>toggleView</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>i4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DViewButton</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(Qt::ToolBarArea area, DToolView *toolView, QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DViewButton</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>(DToolView *toolView, QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DViewButton</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setArea</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(Qt::ToolBarArea area)</arglist>
    </member>
    <member kind="function">
      <type>Qt::ToolBarArea</type>
      <name>area</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a4</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QSize</type>
      <name>sizeHint</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a5</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSensible</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a6</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>blending</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a7</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>DToolView *</type>
      <name>toolView</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>a8</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>paintEvent</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QPaintEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>mousePressEvent</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>b1</anchor>
      <arglist>(QMouseEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>enterEvent</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>b2</anchor>
      <arglist>(QEvent *)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>leaveEvent</name>
      <anchorfile>a00022.html</anchorfile>
      <anchor>b3</anchor>
      <arglist>(QEvent *)</arglist>
    </member>
    <class kind="class">DViewButton::Animator</class>
  </compound>
  <compound kind="class">
    <name>DViewButton::Animator</name>
    <filename>a00023.html</filename>
    <member kind="function">
      <type></type>
      <name>Animator</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QObject *parent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Animator</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>start</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QColor</type>
      <name>blendColors</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(const QColor &amp;color1, const QColor &amp;color2, int percent)</arglist>
    </member>
    <member kind="variable">
      <type>QTimer *</type>
      <name>timer</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>o0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>count</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>o1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>MAXCOUNT</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>o2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>INTERVAL</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>o3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isEnter</name>
      <anchorfile>a00023.html</anchorfile>
      <anchor>o4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DWorkspaceMainWindow</name>
    <filename>a00024.html</filename>
    <base>DMainWindow</base>
    <member kind="function">
      <type></type>
      <name>DWorkspaceMainWindow</name>
      <anchorfile>a00024.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DWorkspaceMainWindow</name>
      <anchorfile>a00024.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addWidget</name>
      <anchorfile>a00024.html</anchorfile>
      <anchor>a2</anchor>
      <arglist>(QWidget *widget, int perspective=DefaultPerspective)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeWidget</name>
      <anchorfile>a00024.html</anchorfile>
      <anchor>a3</anchor>
      <arglist>(QWidget *widget)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventFilter</name>
    <filename>a00025.html</filename>
    <member kind="function">
      <type></type>
      <name>EventFilter</name>
      <anchorfile>a00025.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(DMainWindow *mw, QObject *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventFilter</name>
      <anchorfile>a00025.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>bool</type>
      <name>eventFilter</name>
      <anchorfile>a00025.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QObject *obj, QEvent *event)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TabWidgetPrivate</name>
    <filename>a00026.html</filename>
    <member kind="function">
      <type></type>
      <name>TabWidgetPrivate</name>
      <anchorfile>a00026.html</anchorfile>
      <anchor>a0</anchor>
      <arglist>(QWidget *parent=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TabWidgetPrivate</name>
      <anchorfile>a00026.html</anchorfile>
      <anchor>a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>wheelEvent</name>
      <anchorfile>a00026.html</anchorfile>
      <anchor>b0</anchor>
      <arglist>(QWheelEvent *e)</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>wheelMove</name>
      <anchorfile>a00026.html</anchorfile>
      <anchor>b1</anchor>
      <arglist>(int delta)</arglist>
    </member>
  </compound>
</tagfile>
