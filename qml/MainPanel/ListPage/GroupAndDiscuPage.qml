import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item{
    TabView{
        Tab{
            title: "QQ群"
            GroupList{
                width: parent.width
                height: parent.height
            }
        }
        Tab{
            title: "讨论组"
            RecentList{
                width: parent.width
                height: parent.height
            }
        }
    }
}
