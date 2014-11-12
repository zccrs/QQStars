import QtQuick 2.2
import utility 1.0
import QtQuick.Window 2.1
import QQItemInfo 1.0
import qqstars 1.0

QQ{
    id: root
    property string re_uin: ""//用来存放测试qq是否需要验证码后返回的uin值（密码加密中需要用到）
    property var loginReData//二次登陆后返回的数据(JSON格式)
    property var userData//储存用户资料（JSON格式）
    property var panelSize//存放主面板大小(网络数据)
    property string clientid//存放网络强求需要的clientid
    property var friendListData//储存好友列表
    property string list_hash//获取好友列表时需要的hash
    property string ptwebqq//登录后返回的cookie
    property string psessionid: loginReData?loginReData.psessionid:""//登录后返回的数据
    property string vfwebqq: loginReData?loginReData.vfwebqq:""//登录后返回的数据
    
    windowScale: {
        var dosktopWidth = Screen.desktopAvailableWidth
        if(dosktopWidth<=1366)
            return 1
        else if(dosktopWidth>1366&&dosktopWidth<=1600)
            return 1.2
        else if(dosktopWidth>1600&&dosktopWidth<=1920)
            return 1.4
        else if(dosktopWidth>1920&&dosktopWidth<=2300)
            return 1.6
        else if(dosktopWidth>2300&&dosktopWidth<=2600)
            return 1.8
        else if(dosktopWidth>2600)
            return 2
    }

    onStateChanged: {
        editUserState()//改变在线状态
    }
    
    function random(min,max){
        return Math.floor(min+Math.random()*(max-min));
    }
    function getClientid() {
        return String(random(0, 99)) + String((new Date()).getTime() % 1000000)
    }
    
    function showInputCodePage(callbackFun, uin) {
        var component = Qt.createComponent("../Utility/CodeInput.qml");
        if (component.status == Component.Ready){
            var url = "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+myqq.userQQ+"&cap_cd="+uin
            var data = {"source": url, "backFun":callbackFun};
            var sprite = component.createObject(null, data);
        }
    }
    
    function login(code) {
        if( myqq.loginStatus == QQ.Logining ){
            if( code ) {//开始第一次登陆GET
                var p = encryptionPassword(re_uin, code)
                var url1 = "https://ssl.ptlogin2.qq.com/login?u="+myqq.userQQ+"&p="+p+"&verifycode="+code+"&webqq_type=10&remember_uin=1&login2qq=1&aid=1003903&u1=http%3A%2F%2Fweb2.qq.com%2Floginproxy.html%3Flogin2qq%3D1%26webqq_type%3D10&h=1&ptredirect=0&ptlang=2052&daid=164&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=5-42-29419&mibao_css=m_webqq&t=1&g=1&js_type=0&js_ver=10087&login_sig=0RH3iE1ODTjmJJtKJ5MtDyoG*Q*pwgh2ABgmvw0E0zjdJpjPBbS*H9aZ4WRwLSFk&pt_uistyle=5"
                utility.httpGet(login1Finished, url1)
            }else{//先检测qq号是否需要输入验证码
                utility.socketAbort()//取消以前的网络请求
                var url2 = "https://ssl.ptlogin2.qq.com/check?uin="+myqq.userQQ+"&appid=1003903&r=0.08757076971232891"
                utility.httpGet(testQQFinished, url2)
            }
        }
    }
    function testQQFinished(error, data) {//服务器返回qq是否需要验证码
        if(error){//如果出错了
            login()
            return
        }

        if( myqq.loginStatus == QQ.Logining ){
            var temp = data.split("'")
            re_uin = temp[5]//储存用来加密密码或获取验证码的uin
            if( temp[1]=="0" ){
                login(temp[3])//不需要验证码，直接登录
            }else{
                showCodeWindow(login, temp[3])//调用输入验证码，login为验证码获取成功后的回调函数
            }
        }
    }
    
    function login1Finished(error, data){//登录之后服务器返回的数据
        if(error){//如果出错了
            login(myqq.codeText)//再次请求
            return
        }
        if( myqq.loginStatus == QQ.Logining ){
            var list = data.split ("'");
            if( list[1]==0 ){
                closeCodeWindow()//关闭输入验证码的窗口
                var url = list[5]//先get一下返回数据中的url，来获取必要的Cookie
                utility.httpGet(login2, url)//此地址GET完成后调用二次登录
            }else{
                myqq.showWarningInfo("登录失败："+list[9])
                myqq.error(list[9])
            }
        }
    }
    
    function login2() {
        if( myqq.loginStatus == QQ.Logining ){
            var url = "http://d.web2.qq.com/channel/login2"
            ptwebqq = utility.getCookie("ptwebqq")//储存cookie
            list_hash = getHash()//储存hash
            clientid = getClientid()//设置clientid
            var data = 'r={"status":"'+myqq.stateToString+'","ptwebqq":"'+ptwebqq+'","passwd_sig":"","clientid":"'+clientid+'","psessionid":null}&clientid='+clientid+'&psessionid=null'
            data = encodeURI(data)
            utility.httpPost(login2Finished, url, data)
        }
    }
    function reLogin(){//用于掉线后重新登录
        var url = "http://d.web2.qq.com/channel/login2"
        ptwebqq = utility.getCookie("ptwebqq")//储存cookie
        var data = 'r={"status":"'+myqq.stateToString+'","ptwebqq":"'+ptwebqq+'","passwd_sig":"","clientid":"'+clientid+'","psessionid":null}&clientid='+clientid+'&psessionid=null'
        data = encodeURI(data)
        utility.httpPost(reLoginFinished, url, data, true)
    }
    function reLoginFinished(error, data) {
        if(error){
            reLogin()
            return
        }
        data = JSON.parse(data)
        if( data.retcode==0 ) {
            console.debug("重新登录完成")
            loginReData = data.result//将数据记录下来
            var poll2data = 'r={"clientid":"'+clientid+'","psessionid":"'+psessionid+'","key":0,"ids":[]}&clientid='+clientid+'&psessionid='+psessionid
            myqq.startPoll2(encodeURI(poll2data))//启动心跳包的post
        }else{
            console.debug("重新登录失败")
            showWarningInfo("QQ已掉线，请重新登录")
            root.loginStatus = QQ.WaitLogin//将登录状态设置为离线
        }
    }

    function login2Finished(error, data) {//二次登录，这次才是真正的登录
        if(error){//如果出错了
            login2(null)
            return
        }
        if( myqq.loginStatus == QQ.Logining ){
            var list = JSON.parse(data)
            if( list.retcode==0 ) {
                loginReData = list.result//将数据记录下来
                getUserData(myqq.userQQ, getDataFinished)//获取自己的资料
                myqq.openSqlDatabase();//登录完成后，打开数据库(用来储存聊天记录)
                myqq.loginStatus = QQ.LoginFinished//设置为登录成功
                var poll2data = 'r={"clientid":"'+clientid+'","psessionid":"'+psessionid+'","key":0,"ids":[]}&clientid='+clientid+'&psessionid='+psessionid
                myqq.startPoll2(encodeURI(poll2data))//启动心跳包的post
                var url = "http://q.qlogo.cn/headimg_dl?spec=240&dst_uin="+myqq.userQQ
                downloadImage(QQItemInfo.Friend, url, myqq.userQQ, "240", getAvatarFinished)//获取头像
            }else{
                myqq.showWarningInfo("登陆出错，错误代码："+list.retcode)
            }
        }
    }
    
    function getUserData(uin, backFun) {//获取用户资料，登录完成后的操作
        var url = "http://s.web2.qq.com/api/get_friend_info2?tuin="+uin+"&verifysession=&code=&vfwebqq="+vfwebqq+"&t=1407324674215"
        utility.httpGet(backFun, url, true)//第三个参数为true，是使用高优先级的网络请求
    }
    
    function getDataFinished(error, data) {//获取用户资料成功后
        if(error){//如果出错了
            getUserData(myqq.userQQ, getDataFinished)//再次获取自己的资料
            return
        }
        var list = JSON.parse(data)
        if( list.retcode==0 ) {
            userData = list.result
            //console.debug("获取资料成功，我的昵称是："+userData.nick)
            root.nick = String(userData.nick)//储存昵称
            myqq.addLoginedQQInfo(userQQ, nick)//保存此账号的登录信息
            //getPanelSize()//获取主面板的大小
        }else{
            getUserData(myqq.userQQ, getDataFinished)//再次获取自己的资料
            //myqq.showWarningInfo("获取用户资料出错，错误代码："+list.retcode)
        }
    }
    
    function getPanelSize() {
        if( myqq.loginStatus == QQ.Logining ){
            //var url = "http://cgi.web2.qq.com/keycgi/qqweb/newuac/get.do"
            //var data = 'r={"appid":50,"itemlist":["width","height","defaultMode"]}&uin='+myqq.userQQ
            //data = encodeURI(data)
            //utility.httpPost(getPanelSizeFinished, url, data)
            getPanelSizeFinished(false, "")
        }
    }
    function getPanelSizeFinished ( error, data){
        //var list = JSON.parse(data)
        //if( list.retcode==0 ) {
            //panelSize = list.result//保存获取的数据
        //}else{
            //utility.consoleLog("获取主面板大小出错，错误代码："+list.retcode)
            panelSize = JSON.parse('{"height":500,"defaultMode":"restore","width":240}')
        //}
        
        /*var temp = myqq.value("rememberpassword", 0)==1
        console.log("是否保存密码："+temp)
        if( temp ){//如果要保存密码
            var pass = utility.stringEncrypt(myqq.userPassword, "xingchenQQ")//加密后储存
            
            //myqq.setValue("password", pass)
            console.log("保存的密码为："+myqq.value("password", ""))
        }*/
       
        //myqq.setValue( "nick", userData.nick)//保存昵称
    }
    function getQQSignature(uin, backFun){//获取好友个性签名 backFun为签名获取成功后调用
        var url = "http://s.web2.qq.com/api/get_single_long_nick2?tuin="+uin+"&vfwebqq="+vfwebqq
        utility.httpGet(backFun, url)
    }
    function getFriendList(backFun) {//获取好友列表
        var url = "http://s.web2.qq.com/api/get_user_friends2"
        var data = 'r={"h":"hello","hash":"'+getHash()+'","vfwebqq":"'+vfwebqq+'"}'
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    
    function getGroupList(backFun) {//获取群列表
        var url = "http://s.web2.qq.com/api/get_group_name_list_mask2"
        var data = 'r={"hash":"'+getHash()+'","vfwebqq":"'+vfwebqq+'"}'
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    
    function getRecentList(backFun) {//获取最近联系人
        var url = "http://d.web2.qq.com/channel/get_recent_list2"
        var data = 'r={"vfwebqq":"'+vfwebqq+'","clientid":"'+clientid+'","psessionid":"'+psessionid+'"}&clientid='+clientid+'&psessionid='+psessionid
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    
    function getDiscusList(backFun) {//讨论组列表
        var url = "http://s.web2.qq.com/api/get_discus_list?clientid="+clientid+"&psessionid="+psessionid+"&vfwebqq="+vfwebqq
        utility.httpGet(backFun, url, true)
    }
    
    function getFriendQQ( tuin, backFun ) {//获得好友真实的qq
        var url = "http://s.web2.qq.com/api/get_friend_uin2?tuin="+tuin+"&verifysession=&type=1&code=&vfwebqq="+vfwebqq
        utility.httpGet(backFun, url)
    }
    
    function getAvatarFinished( error, path, name ){//获得自己头像完成
        if(error==DownloadImage.DownloadError){//如果是下载出错
            downloadImage(QQItemInfo.Friend, url, myqq.userQQ, "240", getAvatarFinished)//重新获取头像
            return
        }
        if(error==DownloadImage.NoError)
            myqq.avatar240 = path+"/"+name
    }
    
    function getFriendInfo( tuin,backFun ) {//获取好友资料
        var url = "http://s.web2.qq.com/api/get_friend_info2?tuin="+tuin+"&verifysession=&code=&vfwebqq="+vfwebqq
        utility.httpGet(backFun, url)
    }
    
    function editUserState(){
        if( loginStatus == QQ.LoginFinished ) {
            var url = "http://d.web2.qq.com/channel/change_status2?newstatus="+myqq.stateToString+"&clientid="+clientid+"&psessionid="+psessionid
            utility.httpGet(editUserStateFinished, url)
        }
    }
    function editUserStateFinished(error, data){
        if(error){
            editUserState()//再次请求
            return
        }

        data = JSON.parse(data)
        if( data.retcode==0&&data.result=="ok" ){
            console.log("状态改变成功")
            if(root.state==QQ.Offline){//如果状态变为离线
                root.loginStatus = QQ.WaitLogin//改变登录状态
            }
        }
    }
    
    function sendFriendMessage(backFun, uin, message){
        while(message[message.length-1]=="\n"){
            message = message.substr(0, message.length-1)
        }

        var url = "http://d.web2.qq.com/channel/send_buddy_msg2"
        var data = 'r={"to":'+uin+',"face":549,"content":"[\\"'+message+'\\",\\"\\",[\\"font\\",{\\"name\\":\\"宋体\\",\\"size\\":\\"10\\",\\"style\\":[0,0,0],\\"color\\":\\"000000\\"}]]","msg_id":45070001,"clientid":"'+clientid+'","psessionid":"'+psessionid+'"}&clientid='+clientid+'&psessionid='+psessionid
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    
    function sendGroupMessage(backFun, uin, message){
        while(message[message.length-1]=="\n"){
            message = message.substr(0, message.length-1)
        }

        var url = "http://d.web2.qq.com/channel/send_qun_msg2"
        var data = 'r={"group_uin":'+uin+',"content":"[\\"'+message+'\\",\\"\\",[\\"font\\",{\\"name\\":\\"宋体\\",\\"size\\":\\"10\\",\\"style\\":[0,0,0],\\"color\\":\\"000000\\"}]]","msg_id":29780002,"clientid":"'+clientid+'","psessionid":"'+psessionid+'"}&clientid='+clientid+'&psessionid='+psessionid
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    
    function sendDiscuMessage(backFun, uin, message){
        while(message[message.length-1]=="\n"){
            message = message.substr(0, message.length-1)
        }
        
        var url = "http://d.web2.qq.com/channel/send_discu_msg2"
        var data = 'r={"did":'+uin+',"content":"[\\"'+message+'\\",\\"\\",[\\"font\\",{\\"name\\":\\"宋体\\",\\"size\\":\\"10\\",\\"style\\":[0,0,0],\\"color\\":\\"000000\\"}]]","msg_id":29780002,"clientid":"'+clientid+'","psessionid":"'+psessionid+'"}&clientid='+clientid+'&psessionid='+psessionid
        data = encodeURI(data)
        utility.httpPost(backFun, url, data, true)
    }
    function getGroupMembersList(callbackFun, gcode){//获取群成员列表
        var url = "http://s.web2.qq.com/api/get_group_info_ext2?gcode="+gcode+"&cb=undefined&vfwebqq="+vfwebqq
        utility.httpGet(callbackFun, url, true)
    }
    function getOnlineFriends(callbackFun){//获取在线好友列表
        var url = "http://d.web2.qq.com/channel/get_online_buddies2?clientid="+clientid+"&psessionid="+psessionid
        utility.httpGet(callbackFun, url, true)
    }
    function getDiscusMemberList(callbackFun, did) {//获取讨论组成员列表
        var url = "http://d.web2.qq.com/channel/get_discu_info?did="+did+"&clientid="+clientid+"&psessionid="+psessionid
        utility.httpGet(callbackFun, url, true)
    }
}
