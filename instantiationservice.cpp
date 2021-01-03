#include "instantiationservice.h"

InstantiationService::InstantiationService(IInstantiationService *instantiationService, QObject *parent) :
    IInstantiationService(parent),fParentInstantiationService(instantiationService)
{

}

InstantiationService::~InstantiationService()
{
    qDeleteAll(fSingletons);
}

QSharedPointer<IInstantiationService> InstantiationService::inherited()
{
    return QSharedPointer<IInstantiationService>(new InstantiationService(this));
}

bool InstantiationService::hasObject(const QString &className)
{
    QPointer<QObject> obj = fSingletons.value(className);
    return !obj.isNull();
}

void InstantiationService::registerObject(const QString &className, QObject *object)
{
    fSingletons.insert(className,object);
}

QObject *InstantiationService::queryObject(const QString &className)
{
    if(fSingletons.contains(className)){
        QPointer<QObject> singleton = fSingletons.value(className);
        return singleton.data();
    }
    //当本容器中找不到对应的实例时，如果存在父容器，则去父容器找
    if(!fParentInstantiationService.isNull()){
        return fParentInstantiationService->queryObject(className);
    }
    return nullptr;
}
