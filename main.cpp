#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QObject>
#include <QScreen>
#include <QGuiApplication>
#include <QMenuBar>
#include <QAction>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QComboBox>
#include <QSettings>
#include <QProcess>
#include <QPalette>
#include <QListWidget>
#include <QStackedWidget>
#include <QRadioButton>

///asta doar asa de tupeu daca vrei sa faci cand porneste prima oara sa tina minte ce are nevoie utilizatorul 
///daca are nevoie de un text editor sau de ide (adica sa ai tree de fisiere in stanga , sa ai un terminal jos sa implementezi pt python si pt c++ suport)

int osdetection()
{
    int ostype = 0;
    #ifdef Q_OS_WIN
        ostype = 1; // Windows
    #elif defined(Q_OS_MACOS)
        ostype = 2; // macOS
    #elif defined(Q_OS_LINUX)
        ostype = 3; // Linux
    #else
        ostype = 0; // Unknown OS
    #endif
    return ostype;
}
int ostype = osdetection();

QString detectSystemTheme() {
    switch(ostype) {
        case 1: { // Windows
            QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                               QSettings::NativeFormat);

            if (settings.contains("AppsUseDarkTheme")) {
                int dark = settings.value("AppsUseDarkTheme").toInt();
                if(dark == 1)
                {

                    return "Dark";
                }

            }
            else if (settings.contains("AppsUseLightTheme")) {
                return settings.value("AppsUseLightTheme").toInt() == 1 ? "Light" : "DarkGray";
            }
            return "DarkGray";
        }
        case 2: { // macOS
            QSettings settings("Apple", "Global Preferences");
            return settings.value("AppleInterfaceStyle", "").toString() == "Dark" ? "Dark" : "Light";
        }
        case 3: { // Linux
            // pt GNOME - setari pt desktop pt linux/unix
            QProcess process;
            process.start("gsettings", QStringList() << "get" << "org.gnome.desktop.interface" << "gtk-theme");
            process.waitForFinished();
            QString output = process.readAllStandardOutput();
            return output.contains("dark", Qt::CaseInsensitive) ? "Dark" : "Light";
        }
        default:
            return "DarkGray";
    }
}

void text_lines_characters(const QByteArray& rawData) {
    int characterCount = rawData.size();
    int lineCount = rawData.isEmpty() ? 0 : 1;
    for (char c : rawData) {
        if (c == '\n') lineCount++;
    }
    ///lineCount++;
    ///if (!rawData.isEmpty() && rawData.back() != '\n') {
    ///    lineCount++;
    ///}
    int char_crlf = characterCount + lineCount - 1;;
    qDebug() << "Bytes in file (lf):" << characterCount;
    qDebug() << "Characters (crlf):" << char_crlf << "\n";
    qDebug() << "Lines:" << lineCount;
}

struct Theme {
    QString background;
    QString textColor;
    QString buttonColor;
    QString buttonBorder;
    QString buttonHover;
    QString textEditBg;
    QString borderColor;
};

const Theme DarkContrast {
    "#000000",  // background
    "#ffffff",  // textColor
    "#232323",  // buttonColor
    "#333333",  // buttonBorder
    "#2d2d2d",  // buttonHover
    "#000000",  // textEditBg
    "#333333"   // borderColor
};

const Theme DarkGray {
    "#1e1e1e",  // background
    "#ffffff",  // textColor
    "#1e1e1e",  // buttonColor
    "#666666",  // buttonBorder
    "#505050",  // buttonHover
    "#2b2b2b",  // textEditBg
    "#555555"   // borderColor
};

const Theme Light {
    "#ffffff",  // background
    "#000000",  // textColor
    "#f0f0f0",  // buttonColor
    "#cccccc",  // buttonBorder
    "#e5e5e5",  // buttonHover
    "#ffffff",  // textEditBg
    "#dddddd"   // borderColor
};

const Theme Solarized {
    "#002b36",  // background
    "#839496",  // textColor
    "#073642",  // buttonColor
    "#586e75",  // buttonBorder
    "#094352",  // buttonHover
    "#002b36",  // textEditBg
    "#586e75"   // borderColor
};
/// nu mai fac design sau css in viata mea 
QString createStyleSheet(const Theme& theme) {
    QString arrowImage = (theme.background == Light.background) ? ":/black_down_arrow.png" : ":/arrow_down.png";
    ///vezi ce faci cu scrollbarul pe high contrast black
    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-size: 14px;
        }
        QTextEdit {
            background-color: %6;
            color: %2;
            border: 1px solid %7;
            padding: 6px;
            border-radius: 6px;
        }
        QPushButton {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            padding: 6px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: %5;
        }
        QMenuBar {
            background-color: %1;
            color: %2;
        }
        QMenuBar::item:selected {
            background-color: %5;
        }
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %7;
        }
        QMenu::item:selected {
            background-color: %5;
        }
        QScrollBar:vertical {
            background-color: %1;
            width: 12px;
        }
        QScrollBar::handle:vertical {
            background-color: %3;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: %5;
        }
        QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
        }
        QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical {
            background: %1;
        }
        QComboBox {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            padding: 6px;
            border-radius: 6px;
        }
        QComboBox:hover {
            background-color: %5;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid %4;
        }
        QComboBox::down-arrow {
            image: url(%8);
            width: 15px;
            height: 15px;
        }
        QComboBox QAbstractItemView {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            selection-background-color: %5;
            selection-color: %2;
            outline: none;
        }
        QListWidget {
            background-color: %3;
            border: none;
            padding: 5px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 5px;
        }
        QListWidget::item:selected {
            background-color: %5;
            color: %2;
        }
        QListWidget::item:focus {
            outline: none;
        }
        QListWidget::item:hover {
            background-color: %5;
        }
        QListWidget::item:!active {
            outline: none;
        }
        QStackedWidget {
            background-color: %6;
            /*border: 1px solid %7;*/
            border: none;
            border-radius: 10px;
            padding: 15px;
        }
        QStackedWidget:focus {
            outline: none;
            border: none;
        }
        QComboBox:focus, QComboBox QAbstractItemView:focus {
            outline: none;
            border: 1px solid %4;
        }
        QComboBox QAbstractItemView {
            outline: none;
        } 
    )")
    .arg(theme.background)    // %1
    .arg(theme.textColor)     // %2
    .arg(theme.buttonColor)   // %3
    .arg(theme.buttonBorder)  // %4
    .arg(theme.buttonHover)   // %5
    .arg(theme.textEditBg)    // %6
    .arg(theme.borderColor)   // %7
    .arg(arrowImage);         // %8
}

void applySystemTheme(QApplication& app) {
    QString systemTheme = detectSystemTheme();
    if (systemTheme == "Dark") {
        app.setStyleSheet(createStyleSheet(DarkContrast));
    } else if (systemTheme == "Light") {
        app.setStyleSheet(createStyleSheet(Light));
    }
    else {
        app.setStyleSheet(createStyleSheet(DarkGray));
    }
}
/// nu o sa ma angajeze nimeni vreodata daca programez totul intro singur fisier da nah am vrut si eu sa ma distrez o data
void settings(QMainWindow &window, QApplication &app) {
    QDialog *settingsDialog = new QDialog(&window);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->setWindowTitle("Settings");
    settingsDialog->setFixedSize(600, 400);

    QHBoxLayout *mainlayout = new QHBoxLayout();
    QListWidget *sidebar = new QListWidget();
    sidebar->addItem("General");
    sidebar->addItem("Theme & Colors");
    sidebar->setFixedWidth(180);
    QStackedWidget *stackedWidget = new QStackedWidget();

    ///general settings
    QWidget *general_frame = new QWidget;
    QHBoxLayout *general_layout = new QHBoxLayout(general_frame);
    QRadioButton *text_editor_rad = new QRadioButton("Text Editor");
    QRadioButton *ide_rad = new QRadioButton("IDE");
    general_layout->addWidget(new QLabel("Choose operating a mode:"));
    general_layout->addWidget(text_editor_rad);
    general_layout->addWidget(ide_rad);
    text_editor_rad->setChecked(true);
    stackedWidget->insertWidget(0 , general_frame);

    ///theme settings
    QWidget *theme_frame = new QWidget;
    QVBoxLayout *theme_layout = new QVBoxLayout(theme_frame);
    QHBoxLayout *theme_choose = new QHBoxLayout();
    QLabel *infoLabel = new QLabel("Choose a theme:");
    QComboBox *themeDropdown = new QComboBox();
    theme_choose->setSpacing(5);
    theme_choose->addWidget(infoLabel);
    theme_choose->addWidget(themeDropdown, 1);
    theme_choose->addStretch();
    themeDropdown->addItems({
        "System Theme", "Dark Contrast", "Dark Gray", "Light", "Solarized"
    });
    themeDropdown->setCurrentText("System Theme");
    QObject::connect(themeDropdown, &QComboBox::currentTextChanged, [&app](const QString &themeName) {
        if (themeName == "System Theme") {
            applySystemTheme(app);
        } else if (themeName == "Dark Contrast") {
            app.setStyleSheet(createStyleSheet(DarkContrast));
        } else if (themeName == "Dark Gray") {
            app.setStyleSheet(createStyleSheet(DarkGray));
        } else if (themeName == "Light") {
            app.setStyleSheet(createStyleSheet(Light));
        } else if (themeName == "Solarized") {
            app.setStyleSheet(createStyleSheet(Solarized));
        }
    });
    theme_layout->addLayout(theme_choose);
    theme_layout->addStretch();
    stackedWidget->insertWidget(1, theme_frame);

    sidebar->setCurrentRow(0);
    sidebar->setFocusPolicy(Qt::NoFocus);
    QObject::connect(sidebar, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    mainlayout->addWidget(sidebar);
    mainlayout->addWidget(stackedWidget);
    settingsDialog->setLayout(mainlayout);
    settingsDialog->show(); //exec il face sa nu mai poti interactiona cu restul aplicatiei
    /// show il face doar sa apara si practic se termina imediat pentru ca sa terminat functia
    ///si de aia folosesti cu heap allocation ca sa nu se stearga automat
    ///adauga un buton de reset la setari ca sa revina la default
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;    
    window.setWindowIcon(QIcon(":/icon.png"));
    window.setWindowTitle("Basic ahh Text Editor");

    QString currentFilePath = "";
    QTextEdit textEdit;
    textEdit.setPlaceholderText("Hai la o cafea");
    QPushButton saveButton("Save");
    QPushButton loadButton("Load");
    QPushButton settingsButton("Settings");

    QPalette darkPalette;
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    window.setAutoFillBackground(true);
    app.setPalette(darkPalette);

    QMenuBar menuBar;
    QMenu fileMenu("File", &menuBar);
    QAction newAction("New", &menuBar);
    QAction openAction("Open", &menuBar);
    QAction saveAction("Save", &menuBar);

    QVBoxLayout mainlayout;
    QHBoxLayout buttonlayout;
    
    ///adauga iconita la butoane mrog asta optional , dar adaug o coloana cu nr randului in stanga
    ///adauga in dreapta jos ca la notepad linia si caracterul la care te afli
    ///adauga optiuna/functionalitate de a da quick save gen daca am deschis un fisier cu load atunci cand dau save daca vede ca 
    ///exista deja fisieru sau nu stiu cum testezu dar sa nu mai intre in explorer ca sa faca fisier nou de fiecare data
    ///daca incearca sa inchida aplicatia sa intrebe daca vrea sa salveze modificarile


    QObject::connect(&saveButton, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&window, "Save as", "C:/", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QString content = textEdit.toPlainText();
            
                // Convert to OS-appropriate line endings before saving
                if (ostype == 1) { // Windows
                    // Ensure all line endings are CRLF
                    content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
                } else { 
                    // macOS or Linux - ensure all are LF
                    content.replace("\r\n", "\n").replace("\r", "\n");
                }
                
                QTextStream out(&file);
                out << content;
                file.close();
            } else {
                QMessageBox::warning(&window, "Error", "Could not write to file.");
            }
        }
    });

    QObject::connect(&loadButton , &QPushButton::clicked, [&]()
    {
        QString fileName = QFileDialog::getOpenFileName(&window, "Open File", "C:/", "All Files (*);;C++ files (*.cpp);;Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray rawData = file.readAll();
                text_lines_characters(rawData);
                QString content = QString::fromUtf8(rawData);
                file.close();
                textEdit.setPlainText(content);
                currentFilePath = fileName; // <-- Save path
                if (currentFilePath.isEmpty()) {
                    window.setWindowTitle("Basic ahh Text Editor");
                }
                if (ostype == 1) {
                    currentFilePath.replace("/", "\\");
                }
                window.setWindowTitle(currentFilePath + " - Basic ahh Text Editor");
            } else {
                QMessageBox::warning(&window, "Error", "Could not open file for reading.");
            }
        }
    });

    settingsButton.setToolTip("Settings");
    settingsButton.setText("⚙️");
    ///fa settings fara titlebar si cu fixed size
    ///cand se schimba stylesheetu sa se schimbe si culoarea de la titlebar
    ///adauga optiunea de rounded sau nu la toata aplicatia
    ///ok deci nu imi place cum arata dropdown cand e selectat e foarte naspa blue ala sa modifici in stylesheet
    /// adauga dropdown intrun horizontal layout ca sa fie in dreptul labelui
    /// adauga niste text lineuri in caz ca vrea custom
    /// a adauga 2 radio buttonuri in stanga la label si in stanga labelui custom pt zona custom ca sa stie cand da apply ce setare foloseste
    /// toate border-radius o sa aiba valoarea lor modificata cu text edit
    /// fa sa se mareasca interfata cand apar text lineurile
    /// adauga un buton apply la final si de reset si de close
    ///adauga o modalitate sa salveze ultimele fisiere pe care le ai avut deschise sa le deschida instant si setarile pe care le a ales(ex sia facut tema custom)
    QObject::connect(&settingsButton, &QPushButton::clicked, [&]()
    {
        settings(window, app);
    });

    applySystemTheme(app);
    buttonlayout.addWidget(&saveButton);
    buttonlayout.addWidget(&loadButton);
    buttonlayout.addWidget(&settingsButton);

    mainlayout.addLayout(&buttonlayout);
    mainlayout.addWidget(&textEdit);

    QWidget centralWidget;
    centralWidget.setLayout(&mainlayout);
    window.setCentralWidget(&centralWidget);
    ///window.showMaximized();
    window.setMinimumSize(800, 600);
    window.show();
    return app.exec();
}
