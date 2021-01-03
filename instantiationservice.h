#ifndef INSTANTIATIONSERVICE_H
#define INSTANTIATIONSERVICE_H

#include "iinstantiationservice.h"
#include <QObject>

class InstantiationService : public IInstantiationService
{
    Q_OBJECT
public:
    explicit InstantiationService(IInstantiationService *instantiationService,QObject *parent = nullptr);
    ~InstantiationService();

    QSharedPointer<IInstantiationService> inherited() override;
protected:
    bool hasObject(const QString &className) override;
    void registerObject(const QString &className, QObject *object) override;
    QObject* queryObject(const QString &className) override;
private:
    QHash<QString,QPointer<QObject>> fSingletons;
    QPointer<IInstantiationService> fParentInstantiationService;
};

#endif // INSTANTIATIONSERVICE_H
