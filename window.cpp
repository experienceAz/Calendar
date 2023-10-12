#include "window.h"
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QTextCharFormat>

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    createPreviewGroupBox();
    createDatesGroupBox();
    createTextFormatsGroupBox();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(previewGroupBox, 0, 0);
    layout->addWidget(generalOptionsGroupBox, 0, 1);
    layout->addWidget(datesGroupBox, 1, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    previewLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
    previewLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());

    setWindowTitle(tr("Календарь"));
}


void Window::firstDayChanged(int index)
{
    calendar->setFirstDayOfWeek(Qt::DayOfWeek(firstDayCombo->itemData(index).toInt()));
}

void Window::selectionModeChanged(int index)
{
    calendar->setSelectionMode(QCalendarWidget::SelectionMode(selectionModeCombo->itemData(index).toInt()));
}

void Window::horizontalHeaderChanged(int index)
{
    calendar->setHorizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat(horizontalHeaderCombo->itemData(index).toInt()));
}

void Window::verticalHeaderChanged(int index)
{
    calendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat(verticalHeaderCombo->itemData(index).toInt()));
}

void Window::selectedDateChanged()
{
    currentDateEdit->setDate(calendar->selectedDate());
    currentDateLabel->setText(currentDateEdit->text());
}

void Window::weekdayFormatChanged()
{
    QTextCharFormat format;

    format.setForeground(qvariant_cast<QColor>(
    weekdayColorCombo->itemData(weekdayColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Monday, format);
    calendar->setWeekdayTextFormat(Qt::Tuesday, format);
    calendar->setWeekdayTextFormat(Qt::Wednesday, format);
    calendar->setWeekdayTextFormat(Qt::Thursday, format);
    calendar->setWeekdayTextFormat(Qt::Friday, format);
}

void Window::weekendFormatChanged()
{
    QTextCharFormat format;

    format.setForeground(qvariant_cast<QColor>(weekendColorCombo->itemData(weekendColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Saturday, format);
    calendar->setWeekdayTextFormat(Qt::Sunday, format);
}

void Window::reformatHeaders()
{
    QString text = headerTextFormatCombo->currentText();
    QTextCharFormat format;

    if (text == tr("Bold"))
        format.setFontWeight(QFont::Bold);
    else if (text == tr("Italic"))
        format.setFontItalic(true);
    else if (text == tr("Green"))
        format.setForeground(Qt::green);
    calendar->setHeaderTextFormat(format);
}

void Window::reformatCalendarPage()
{
    QTextCharFormat mayFirstFormat;
    const QDate mayFirst(calendar->yearShown(), 5, 1);

    QTextCharFormat firstFridayFormat;
    QDate firstFriday(calendar->yearShown(), calendar->monthShown(), 1);
    while (firstFriday.dayOfWeek() != Qt::Friday)
        firstFriday = firstFriday.addDays(1);

    if (firstFridayCheckBox->isChecked()) {
        firstFridayFormat.setForeground(Qt::blue);
    } else {
        Qt::DayOfWeek dayOfWeek(static_cast<Qt::DayOfWeek>(firstFriday.dayOfWeek()));
        firstFridayFormat.setForeground(calendar->weekdayTextFormat(dayOfWeek).foreground());
    }

    calendar->setDateTextFormat(firstFriday, firstFridayFormat);

    if (mayFirstCheckBox->isChecked()) {
        mayFirstFormat.setForeground(Qt::red);
    } else if (!firstFridayCheckBox->isChecked() || firstFriday != mayFirst) {
        Qt::DayOfWeek dayOfWeek(static_cast<Qt::DayOfWeek>(mayFirst.dayOfWeek()));
        calendar->setDateTextFormat(mayFirst, calendar->weekdayTextFormat(dayOfWeek));
    }

    calendar->setDateTextFormat(mayFirst, mayFirstFormat);
}

void Window::createPreviewGroupBox()
{
    previewGroupBox = new QGroupBox();

    calendar = new QCalendarWidget;
    calendar->setMinimumDate(QDate(1900, 1, 1));
    calendar->setMaximumDate(QDate(3000, 1, 1));
    calendar->setGridVisible(true);

    connect(calendar, &QCalendarWidget::currentPageChanged, this, &Window::reformatCalendarPage);

    previewLayout = new QGridLayout;
    previewLayout->addWidget(calendar, 0, 0, Qt::AlignCenter);
    previewGroupBox->setLayout(previewLayout);
}

void Window::createDatesGroupBox()
{
    datesGroupBox = new QGroupBox();

    currentDateEdit = new QDateEdit;
    currentDateEdit->setDisplayFormat("MMM d yyyy");
    currentDateEdit->setDate(calendar->selectedDate());
    currentDateEdit->setDateRange(calendar->minimumDate(), calendar->maximumDate());

    currentDateLabel = new QLabel();
    currentDateLabel->setText(currentDateEdit->text());

    connect(calendar, &QCalendarWidget::selectionChanged, this, &Window::selectedDateChanged);

    QGridLayout *dateBoxLayout = new QGridLayout;
    dateBoxLayout->addWidget(currentDateLabel);
    dateBoxLayout->setAlignment(currentDateLabel, Qt::AlignRight);
    datesGroupBox->setLayout(dateBoxLayout);
}

void Window::createTextFormatsGroupBox()
{
    weekdayColorCombo = createColorComboBox();
    weekdayColorCombo->setCurrentIndex(
    weekdayColorCombo->findText(tr("Black")));

    weekendColorCombo = createColorComboBox();
    weekendColorCombo->setCurrentIndex(
    weekendColorCombo->findText(tr("Red")));

    headerTextFormatCombo = new QComboBox;
    headerTextFormatCombo->addItem(tr("Bold"));
    headerTextFormatCombo->addItem(tr("Italic"));
    headerTextFormatCombo->addItem(tr("Plain"));

    firstFridayCheckBox = new QCheckBox();
    mayFirstCheckBox = new QCheckBox();

    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::weekdayFormatChanged);
    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatCalendarPage);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::weekendFormatChanged);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatCalendarPage);
    connect(headerTextFormatCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatHeaders);
    connect(firstFridayCheckBox, &QCheckBox::toggled,
            this, &Window::reformatCalendarPage);
    connect(mayFirstCheckBox, &QCheckBox::toggled,
            this, &Window::reformatCalendarPage);

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(firstFridayCheckBox);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(mayFirstCheckBox);

    weekendFormatChanged();
    reformatHeaders();
    reformatCalendarPage();
}

QComboBox *Window::createColorComboBox()
{
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Red"), QColor(Qt::red));
    comboBox->addItem(tr("Blue"), QColor(Qt::blue));
    comboBox->addItem(tr("Black"), QColor(Qt::black));
    comboBox->addItem(tr("Magenta"), QColor(Qt::magenta));
    return comboBox;
}
