#ifndef SEEKERPOSITION_H
#define SEEKERPOSITION_H

#include <QObject>

class SeekerPosition : public QObject
{
    Q_OBJECT;
public:
    SeekerPosition();
    SeekerPosition(float val);
    void set(float val);
    float get();

public slots:
    void setPosition(qint64 pos);

signals:
    void positionChanged(float pos);

private:
    float _position;
};

#endif // SEEKERPOSITION_H
