#ifndef IINSTANTIATIONSERVICE_H
#define IINSTANTIATIONSERVICE_H

#define IOC_INJECTABLE(CLASSNAME,fieldName) \
    Q_INVOKABLE bool __inspect##CLASSNAME(QObject *fieldName){\
        if(QString(#CLASSNAME) != fieldName->metaObject()->className()){\
            return false;\
        }\
        this->fieldName = reinterpret_cast<CLASSNAME *>(fieldName);\
        return true;\
    }

#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QHash>
#include <QDebug>

class IInstantiationService : public QObject
{
    Q_OBJECT
public:
    explicit IInstantiationService(QObject *parent = nullptr)
        :QObject(parent){}

    virtual ~IInstantiationService(){}

    template<typename T>
    bool registerSingleton(T *instance){
        QString className = instance->metaObject()->className();
        bool alreadyExists = hasObject(className);
        if(alreadyExists){
            qWarning() << "The singleton with class : " << className << " already exists in DI container!";
            return false;
        }
        registerObject(className,instance);
        emit objectAdded(className,instance);
        return true;
    }

    template<typename T>
    T* getSingleton(){
        QString className = T::metaObject().className();
        QObject *obj = queryObject(className);
        if(obj == nullptr){
            return nullptr;
        }
        T *singleton = qobject_cast<T*>(obj);
        return singleton;
    }

    template<typename T,typename... Args>
    T* newObject(Args... args){
        T *obj = new T(args...);
        const QMetaObject *metaObj = obj->metaObject();
        for(int i = 0; i < metaObj->methodCount();i++){
            QMetaMethod method = metaObj->method(i);
            if(!method.isValid()){
                continue;
            }
            QString injectMethodPrefix = "__inspect";
            QString methodName = method.name();
            if(!methodName.startsWith(injectMethodPrefix)){
                continue;
            }
            QString autoInjectType = methodName.mid(injectMethodPrefix.length());
            QObject *instance = queryObject(autoInjectType);
            bool injectResult = false;
            if(instance){
                method.invoke(obj,Qt::DirectConnection,Q_RETURN_ARG(bool,injectResult),Q_ARG(QObject *,instance));
            }
            if(injectResult){
                qDebug() << "Auto inject type[" << autoInjectType << "] for object :" << metaObj->className() << " successed.";
            }else{
                qWarning() << "Auto inject type[" << autoInjectType << "] for object :" << metaObj->className() << " failed.";
            }
        }
        return obj;
    }

    virtual QSharedPointer<IInstantiationService> inherited() = 0;
signals:
    void objectAdded(QString className,QObject *obj);
protected:
    virtual bool hasObject(const QString &className) = 0;
    virtual void registerObject(const QString &className,QObject *object) = 0;
    virtual QObject* queryObject(const QString &className) = 0;
private:

    friend class InstantiationService;
};

#endif // IINSTANTIATIONSERVICE_H
