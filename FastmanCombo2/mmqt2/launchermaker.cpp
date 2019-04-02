#include "launchermaker.h"
#include "launcherconfig.h"
#include "launchereditor.h"
#include "zlog.h"
#include <QUuid>
#include <QProcess>
#include <stdio.h>
#include <QFile>
#include "zstringutil.h"

LauncherMaker::LauncherMaker(LauncherConfig *config, const char *serial)
    :_config(config),_serial(serial)
{
    _editor = new LauncherEditor();

}

void LauncherMaker::init()
{
    // �ļ�����Ϣ
    for(int it = 0;it != _config->_folderOrder.size();++it){
        const PosNode& n = _config->_folderOrder.at(it);
        _editor->addFolder(n.screen,n.x,n.y,n.name.toUtf8().data(),n.containID,n.istrashcan,n.name);
    }
    // ϵͳ�Ҽ���Ϣ
    for(int it = 0;it != _config->_widgetOrder.size();++it){
        const PosNode& n = _config->_widgetOrder.at(it);
        _editor->addWidget(n.screen,n.x,n.y,n.xn,n.yn,n.pname.toUtf8().data(),n.cname.toUtf8().data());
    }
    // �̶�Ӧ��λ����Ϣ
    for(int it = 0;it != _config->_fixedPkg.size();++it){
        const PosNode& n = _config->_fixedPkg.at(it);
        _editor->addPkg(n.screen,n.x,n.y,n.pname.toUtf8().data(),n.cname.toUtf8().data());
    }
}

bool LauncherMaker::sync()
{
    _editor->sUninstallList = _config->sUninstallList;

    DBG("fool.\n");
    int currentPos = 0;
    _editor->clearPkg();
    // �̶�Ӧ��λ����Ϣ
    DBG("clear _editor,do with fixedpkg.\n");
    for(int it = 0;it != _config->_fixedPkg.size();++it){
        const PosNode& n = _config->_fixedPkg.at(it);
        _editor->addPkg(n.screen,n.x,n.y,n.pname.toUtf8().data(),n.cname.toUtf8().data());
    }
    DBG("_fixedPkg.size = %d.\n",_config->_fixedPkg.size());

    QFile file("ini.txt");
    file.resize(0);

    foreach(const AGENT_FILTER_INFO *filter_node,_editor->m_filter){
        QString label = filter_node->label;
        if(label.contains(ZStringUtil::fromGBK("�̵�")) || label.contains(ZStringUtil::fromGBK("�г�"))){
            _editor->appmarket.append(filter_node->pkg);
        }
        for(int it = 0;it != _config->_replaceOrder.size();++it){
            const ReplaceNode& rapk = _config->_replaceOrder.at(it);
            QStringList list = rapk.lName.split(",");
            if(list.size() > 0){
                for(int j=0;j<list.size();j++){
                    if(list.at(j) == label){
                        RPkg *rpkg = new RPkg;
                        rpkg->fromclassname = filter_node->name;
                        rpkg->frompkgname = filter_node->pkg;
                        rpkg->toclassname = rapk.classname;
                        rpkg->topkgname = rapk.pkgname;
                        _editor->_ReplaceOrder.append(rpkg);
                        if(file.open(QFile::WriteOnly | QFile::Text | QFile::Append)){
                            QString str = rpkg->toclassname + "&" +list.at(j) +"\n";
                            file.write(str.toUtf8().data());
                            file.close();
                        }
                    }
                }
            }
        }
    }
    QStringList listTC;
    if(!_config->_posOrder.isEmpty()){
        PosNode node = _config->_posOrder.at(currentPos);
        int PosOrderSize = _config->_posOrder.size();
        //���պ�̨���õ�apk˳�򽫰�װ��Ӧ��(��̨��ָ����)����
        //ע������Ϊ�����������λ�ö���Ԥ��ָ���ģ����ⷢ�����渲��
        while(currentPos < PosOrderSize)
        {
            node = _config->_posOrder.at(currentPos);
            if (node.containID == -101){
                //���ײ��Ĵ������õ�
                BPkg *bPkg = new BPkg;
                bPkg->x = node.x;
                bPkg->y = node.y;

                if(node.type == "SYS"){
                    foreach(const AGENT_FILTER_INFO *filter_node,_editor->m_filter)
                    {
                        QString label = filter_node->label;
                        QString name = node.name;
                        QStringList namelist = name.split(",");
                        if(namelist.size()>0){
                            for(int j = 0;j<namelist.size();j++){
                                if (label == namelist.at(j)){
                                    node.cname = filter_node->name.toUtf8().data();
                                    node.pname = filter_node->pkg.toUtf8().data();
                                    break;
                                }
                            }
                        }
                    }
                }

                bPkg->classname = node.cname;
                bPkg->pkgname = node.pname;

                _editor->_bottomOrder.append(bPkg);
            }else if(node.type == "SYS"){
                // �ֻ�����ԭ��������Ӧ�ã�һ����ϵͳ��
                QStringList Label;
                QString market = "900�г�,��ΪӦ���г�,Ӧ���г�,����Ӧ���̵�,Ӧ���̵�,����̵�,С���̳�";
                QString game = "��Ϊ��Ϸ����,��Ϸ����,��Ϸ����,��Ϸ";

                foreach(const AGENT_FILTER_INFO *filter_node,_editor->m_filter)
                {
                    QString label = filter_node->label;
                    Label.append(label);
                    QString name = node.name;
                    QStringList namelist = name.split(",");
                    if(namelist.size()>0){
                        for(int j = 0;j<namelist.size();j++){
                            if (label == namelist.at(j)){
                                int a = 221;
                                _editor->addPkg(node.screen,node.x,node.y,filter_node->pkg.toUtf8().data(),filter_node->name.toUtf8().data());
                                node.pname = filter_node->pkg;
                                node.cname = filter_node->name;
                                break;
                            }
                            QStringList mlist = market.split(",");
                            if(mlist.size()>0){
                                for(int i=0; i<mlist.size(); i++){
                                    QString Market = mlist.at(i);
                                    if(namelist.at(j) == ZStringUtil::fromGBK(Market.toLatin1())){
                                        QString cname = "com.zxly.market.activity.SplashActivity";
                                        QString pname = "com.shyz.desktop";
                                        _editor->addPkg(node.screen,node.x,node.y,pname.toUtf8().data(),cname.toUtf8().data());
                                        node.pname = pname;
                                        node.cname = cname;
                                    }
                                }
                            }
                            QStringList glist = game.split(",");
                            if(glist.size()>0){
                                for(int i=0; i<glist.size(); i++){
                                    QString Game = glist.at(i);
                                    if(namelist.at(j) == ZStringUtil::fromGBK(Game.toLatin1())){
                                        QString cname = "com.shyz.desktop.activity.GameCenterActivity";
                                        QString pname = "com.shyz.desktop";
                                        _editor->addPkg(node.screen,node.x,node.y,pname.toUtf8().data(),cname.toUtf8().data());
                                        node.pname = pname;
                                        node.cname = cname;
                                    }
                                }
                            }
                        }
                    }
                }
                QString name = node.name;
                QStringList namelist = name.split(",");
                if(namelist.size() == 1){
                    if(!Label.contains(namelist.at(0))){
                        _editor->addPkg(node.screen,node.x,node.y,node.pname.toUtf8().data(),node.cname.toUtf8().data());
                    }
                }
                int flag = 0;
                if(namelist.size()>1){
                    for(int it=0 ;it!=namelist.size(); ++it){
                        if(Label.contains(namelist.at(it))){
                            flag = 1;
                        }
                    }
                    if(flag == 0){
                        _editor->addPkg(node.screen,node.x,node.y,node.pname.toUtf8().data(),node.cname.toUtf8().data());
                    }
                }
            }else{
                //
                _editor->addPkg(node.screen,node.x,node.y,node.pname.toUtf8().data(),node.cname.toUtf8().data());

            }
            currentPos ++;
            if((node.cname == "")||(node.pname == "")){
                int a = 21;
            }
        }

        //
        for(int it = 0; it != _hiddenOrder.size();++it){
            const HiddenNode& apk = _hiddenOrder.at(it);
            for(int it = 0;it != _editor->folders_.size();++it){
                LauncherEditor::Folder* f = _editor->folders_.at(it);
                if(f->istrashcan == 1){
                    FPkg *fpkg = new FPkg;
                    fpkg->classname = apk.classname;
                    fpkg->pkgname = apk.pkgname;
                    f->_FPkg.append(fpkg);
                    _editor->hiddenPkg.append(apk.pkgname);
                    _editor->_AllFoldPkg.append(fpkg);
                    break;
                }
            }
        }
        //_pkgOrder�����������ڴ����ļ����е����� ��Ϊ_posOrder��һ���Ѿ�ֱ���ṩ�˰���������
        QString s_sim = "";
        for(int it = 0;it != _config->_pkgOrder.size();++it){
            const PkgNode& apk = _config->_pkgOrder.at(it);
            listTC.append(apk.pkgname);
            if(_apps.contains(apk.pkgname)){//������װ����
                QString  key = _apps.value(apk.pkgname);
                if (key == ""){
                    continue;
                }
                if(apk.inside){
                    for(int it = 0;it != _editor->folders_.size();++it)
                    {
                        LauncherEditor::Folder* f = _editor->folders_.at(it);
                        if(apk.foldername == f->sName)
                        {
                            FPkg *fpkg = new FPkg;
                            fpkg->classname = key;
                            fpkg->pkgname = apk.pkgname;
                            f->_FPkg.append(fpkg);
                            _editor->_AllFoldPkg.append(fpkg);
                        }
                    }
                }else if(!apk.inside)
                {
                    for(int it = 0;it != _editor->folders_.size();++it)
                    {
                        LauncherEditor::Folder* f = _editor->folders_.at(it);
                        if(f->istrashcan == 1)
                        {
                            FPkg *fpkg = new FPkg;
                            fpkg->classname = key;
                            fpkg->pkgname = apk.pkgname;
                            f->_FPkg.append(fpkg);
                            _editor->_AllFoldPkg.append(fpkg);
                            break;
                        }
                    }
                }
            }else if(apk.inside){ //��������Ǳ�ʾ����Ӧ��������������ʾ��ϵͳ�Դ�Ӧ��
                for(int it = 0;it != _editor->folders_.size();++it) //���ѭ����Ϊ�˰��ļ��з������ļ�
                {
                    LauncherEditor::Folder* f = _editor->folders_.at(it);
                    if(apk.foldername == f->sName)
                    {
                        FPkg *fpkg = new FPkg;
                        int sim_flag = 0;
                        foreach(const AGENT_FILTER_INFO *filter_node,_editor->m_filter)
                        {
                            QString label = filter_node->label;
                            if (label == apk.pkgname){
                                if(apk.pkgname == ZStringUtil::fromGBK("SIM ��Ӧ��")) //ȥ��
                                {
                                    if(s_sim == "")
                                    {
                                        s_sim = filter_node->name;  //����ȥ��
                                    }else if(filter_node->name == s_sim){
                                        continue;
                                    }
                                }

                                fpkg->classname = filter_node->name;
                                fpkg->pkgname = filter_node->pkg;
                                break;
                            }
                            else if((apk.pkgname == ZStringUtil::fromGBK("SIM ��Ӧ��1"))
                                    &&(label == ZStringUtil::fromGBK("SIM ��Ӧ��"))){ //��һ��ȥ�ظ��жϣ�
                                if(s_sim == "")
                                {
                                    s_sim = filter_node->name;  //����ȥ��
                                }else if(filter_node->name == s_sim){
                                    continue;
                                }

                                fpkg->classname = filter_node->name;
                                fpkg->pkgname = filter_node->pkg;
                                break;
                            }
                        }
                        if(fpkg->pkgname!=""){  //����������ֵ�ԭ������Ϊ��QQ���֣�û������ж�ؽ׶εķ�ж�ذ�����������Ӧ�������ý׶��Ѿ������ڵ���
                            f->_FPkg.append(fpkg);
                            _editor->_AllFoldPkg.append(fpkg);
                        }
                    }
                }

            }


        }


        //���������ն��Ĳ���
        if( currentPos != PosOrderSize ){
            //���ȴӱ�־λ����Ͱ�ĵط����� ����outapps
            QStringList keys = _config->_trashcanapps.keys();
            foreach(const QString& key,keys){
                _editor->addPkg(node.screen,node.x,node.y,key.toUtf8().data(), _config->_trashcanapps.value(key).toUtf8().data());
                currentPos ++;
                if(currentPos != PosOrderSize){
                    node = _config->_posOrder.at(currentPos);
                }
                else {
                    break;
                }
            }
        }

        DBG("_outapps!");
        // ��� ���õ�λ��δ����������ӱ�ѡ��_outapps�в���
        if( currentPos != PosOrderSize ){
            QStringList keys = _outapps.keys();
            foreach(const QString& key,keys){
                _editor->addPkg(node.screen,node.x,node.y,key.toUtf8().data(),_outapps.value(key).toUtf8().data());
                currentPos ++;
                if(currentPos != PosOrderSize){
                    node = _config->_posOrder.at(currentPos);
                }
                else {
                    break;
                }
            }
        }
        DBG("_pushapps!");
        // ��� ���õ�λ��δ�����������_pushapps�в���
        if( currentPos != PosOrderSize ){
            QStringList keys = _pushapps.keys();
            foreach(const QString& key,keys){
                _editor->addUPkg(node.screen,node.x,node.y,_pushapps.value(key).toUtf8().data());
                currentPos ++;
                if(currentPos != PosOrderSize){
                    node = _config->_posOrder.at(currentPos);
                }
                else {
                    break;
                }
            }
        }

        if(!_editor->validate()){ //Ҫ����������ͼ��.
            //DBG("Desktop config is invalid!");
            //return false;
        }
    }
    else {
        DBG("Desktop config is empty!");
        return false;
    }

    DBG("Desktop config is not empty.\n");
    QString configname = QUuid::createUuid().toString() + "_workspace.xml";
    m_configName = configname;
    _editor->autoFolder(false);
    //װ��һ���ײ�Ӧ�õ��б������ڿ���
    _editor->listTC = listTC;
    _editor->Brand = _config->Brand;
    DBG("%s begin to commit!\n",configname.toUtf8().data());
    _editor->commit(configname.toUtf8().data());
    _editor->recommit(configname.toUtf8().data());

    QString ini = QUuid::createUuid().toString() + "_desktopini.xml";
    m_ini = ini;
    _editor->desktopCommit(ini.toUtf8().data());

    QString hiddenConf = QUuid::createUuid().toString() + "_config.xml";
    m_hiddenConf = hiddenConf;
    _editor->hiddenCommit(hiddenConf.toUtf8().data());

    return true;
}



bool LauncherMaker::install(const char *package, const char *path)
{
    bool inside = false,contains = false;
    /*
    if(!_config->contains(package)){
    return false;
    }
    */
    contains = _config->contains(package);
    DBG("add install apk: %s %s", package, path);
    QString stag = "launchable-activity: name='";
    QString line = "aapt d badging %1 | findstr launchable-activity: name='";
    QString className = "";
    QString command = line.arg(path);
    QProcess cmd;
    cmd.start(command);;
    if(cmd.waitForStarted()){
        if(cmd.Running == cmd.state()){
            cmd.waitForFinished();
        }
    }
    QString output = cmd.readAllStandardOutput();
    int idx = output.indexOf(stag);
    if(idx != -1 ){
        int len = output.indexOf("'",idx+stag.length()) - (idx + stag.length());
        className = output.mid(idx + stag.length(),len);
    }
    QString folderName = "";
    inside = _config->inside(package,folderName);
    if(inside){
        LauncherEditor::Folder* f = _editor->search(folderName.toLocal8Bit().data());
        if(f != NULL){
            DBG("add install apk: %s %s, folder: %s", package, path, f->fname);
            _editor->addPkg(0,0,0,package,className.toUtf8().data(),f);
        }
        else
        {
            DBG("add install apk failed: %s %s, folder: %s", package, path, folderName.toUtf8().data());
        }
    }
    else {
        //        if(_curPos != _config->size()){
        //            const LauncherConfig::PosNode& n = _config->at(_curPos);
        //            _editor->addPkg(1,1,1,package,className.toUtf8().data());
        //            _curPos;
        //        }
        DBG("add install apk:contains:%s, %s %s",contains?"Y":"N",package, path);
        if(contains){
            _apps.insert(package,className);
        }
        else {
            // ���APKδ�� ��̨�����������ã��ȱ������á���
            _outapps.insert(package,className);
        }
    }
    return true;
}

void LauncherMaker::folder( bool auto_ /*= true*/ )
{
    _editor->autoFolder(auto_);
}


bool LauncherMaker::push( const char* package,const char* rpath )
{
    bool inside = false,contains = false;
    /*
    if(!_config->contains(package)){
    return false;
    }
    */
    contains = _config->contains(package);
    DBG("add push apk: %s %s", package, rpath);
    QString folderName = "";
    inside = _config->inside(package,folderName);
    if(inside){
        LauncherEditor::Folder* f = _editor->search(folderName.toLocal8Bit().data());
        if(f != NULL){
            DBG("add push apk: %s %s, folder: %s", package, rpath, f->fname);
            _editor->addUPkg(0,0,0,rpath,f);
        }
        else
        {
            DBG("add push apk failed: %s %s, folder: %s", package, rpath, folderName.toUtf8().data());
        }
    }
    else {
        //        if(_curPos != _config->size()){
        //            const LauncherConfig::PosNode& n = _config->at(_curPos);
        //            _editor->addPkg(1,1,1,package,className.toUtf8().data());
        //            _curPos;
        //        }
        DBG("add push apk:contains:%s, %s %s",contains?"Y":"N",package, rpath);
        _pushapps.insert(package,rpath);
    }
    return true;
}

void LauncherMaker::InsertAPK(QString package,QString ClassName)
{
    _apps.insert(package,ClassName);
}
