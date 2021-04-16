#ifndef INFO_H
#define INFO_H
#include <QVector>
class JsonInfo
{
public:
    QVector<QString> m_Songname_original; //歌曲名
    QVector<QString> m_Singername;        //歌手
    QVector<QString> m_Album_name;        //专辑
    QVector<int>     m_Duration;          //时间
    QVector<QString> m_Hash;              //哈希值
    QVector<QString> m_Album_id;          //歌曲id
    QVector<int>     m_play;              //次数

public:
    void clear_info(){
        m_Songname_original.clear();
        m_Singername.clear();
        m_Album_name.clear();
        m_Duration.clear();
        m_Hash.clear();
        m_Album_id.clear();
        m_play.clear();
    }
};
#endif // INFO_H
