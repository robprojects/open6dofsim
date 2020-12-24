#pragma once
#include "qcombobox.h"
#include "qpushbutton.h"

class Selectable {
public:
    virtual QString& name() = 0;
    virtual void openConfigDialog() {

    }
    virtual void setActive() {

    }
    virtual void setInactive() {

    }
    virtual QString &getStatus() = 0;
};

class Selector : public QObject {
    Q_OBJECT
public:
    Selector(QComboBox* box, QPushButton *config) : QObject(box), m_box(box), m_current(NULL) {
        connect(config, SIGNAL(clicked()), this, SLOT(openConfigDialog()));
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(inputSelect(int)));
    }
    void addOption(Selectable *s, bool default = false) {
        m_box->addItem(s->name());
        m_items.push_back(s);
        if (default) {
            int index = m_items.size() - 1;
            m_box->setCurrentIndex(index);
            inputSelect(index);
        }
    }
    Selectable* getCurrent() {
        return m_current;
    }

    QString& getStatus() {
        static QString none("");
        if (!m_current) return none;
        return m_current->getStatus();
    }

public slots:
    void openConfigDialog() {
        if (m_current) m_current->openConfigDialog();
    }
    void inputSelect(int index) {
        if (index >= m_items.size()) return;
        if (m_current) m_current->setInactive();
        m_current = m_items[index];
        m_current->setActive();
    }

private:
    QVector<Selectable *> m_items;
    Selectable* m_current;
    QComboBox* m_box;
};