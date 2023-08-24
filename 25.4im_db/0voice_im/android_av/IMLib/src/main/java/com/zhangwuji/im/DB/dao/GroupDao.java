package com.zhangwuji.im.DB.dao;

import android.database.Cursor;
import android.database.sqlite.SQLiteStatement;

import org.greenrobot.greendao.AbstractDao;
import org.greenrobot.greendao.Property;
import org.greenrobot.greendao.internal.DaoConfig;
import org.greenrobot.greendao.database.Database;
import org.greenrobot.greendao.database.DatabaseStatement;

import com.zhangwuji.im.DB.entity.Group;

// THIS CODE IS GENERATED BY greenDAO, DO NOT EDIT.
/** 
 * DAO for table "GroupInfo".
*/
public class GroupDao extends AbstractDao<Group, Long> {

    public static final String TABLENAME = "GroupInfo";

    /**
     * Properties of entity Group.<br/>
     * Can be used for QueryBuilder and for referencing column names.
     */
    public static class Properties {
        public final static Property Id = new Property(0, Long.class, "id", true, "_id");
        public final static Property PeerId = new Property(1, int.class, "peerId", false, "PEER_ID");
        public final static Property GroupType = new Property(2, int.class, "groupType", false, "GROUP_TYPE");
        public final static Property MainName = new Property(3, String.class, "mainName", false, "MAIN_NAME");
        public final static Property Avatar = new Property(4, String.class, "avatar", false, "AVATAR");
        public final static Property CreatorId = new Property(5, int.class, "creatorId", false, "CREATOR_ID");
        public final static Property UserCnt = new Property(6, int.class, "userCnt", false, "USER_CNT");
        public final static Property UserList = new Property(7, String.class, "userList", false, "USER_LIST");
        public final static Property Version = new Property(8, int.class, "version", false, "VERSION");
        public final static Property Status = new Property(9, int.class, "status", false, "STATUS");
        public final static Property Created = new Property(10, int.class, "created", false, "CREATED");
        public final static Property Updated = new Property(11, int.class, "updated", false, "UPDATED");
    }


    public GroupDao(DaoConfig config) {
        super(config);
    }
    
    public GroupDao(DaoConfig config, DaoSession daoSession) {
        super(config, daoSession);
    }

    /** Creates the underlying database table. */
    public static void createTable(Database db, boolean ifNotExists) {
        String constraint = ifNotExists? "IF NOT EXISTS ": "";
        db.execSQL("CREATE TABLE " + constraint + "\"GroupInfo\" (" + //
                "\"_id\" INTEGER PRIMARY KEY AUTOINCREMENT ," + // 0: id
                "\"PEER_ID\" INTEGER NOT NULL UNIQUE ," + // 1: peerId
                "\"GROUP_TYPE\" INTEGER NOT NULL ," + // 2: groupType
                "\"MAIN_NAME\" TEXT NOT NULL ," + // 3: mainName
                "\"AVATAR\" TEXT NOT NULL ," + // 4: avatar
                "\"CREATOR_ID\" INTEGER NOT NULL ," + // 5: creatorId
                "\"USER_CNT\" INTEGER NOT NULL ," + // 6: userCnt
                "\"USER_LIST\" TEXT NOT NULL ," + // 7: userList
                "\"VERSION\" INTEGER NOT NULL ," + // 8: version
                "\"STATUS\" INTEGER NOT NULL ," + // 9: status
                "\"CREATED\" INTEGER NOT NULL ," + // 10: created
                "\"UPDATED\" INTEGER NOT NULL );"); // 11: updated
    }

    /** Drops the underlying database table. */
    public static void dropTable(Database db, boolean ifExists) {
        String sql = "DROP TABLE " + (ifExists ? "IF EXISTS " : "") + "\"GroupInfo\"";
        db.execSQL(sql);
    }

    @Override
    protected final void bindValues(DatabaseStatement stmt, Group entity) {
        stmt.clearBindings();
 
        Long id = entity.getId();
        if (id != null) {
            stmt.bindLong(1, id);
        }
        stmt.bindLong(2, entity.getPeerId());
        stmt.bindLong(3, entity.getGroupType());
        stmt.bindString(4, entity.getMainName());
        stmt.bindString(5, entity.getAvatar());
        stmt.bindLong(6, entity.getCreatorId());
        stmt.bindLong(7, entity.getUserCnt());
        stmt.bindString(8, entity.getUserList());
        stmt.bindLong(9, entity.getVersion());
        stmt.bindLong(10, entity.getStatus());
        stmt.bindLong(11, entity.getCreated());
        stmt.bindLong(12, entity.getUpdated());
    }

    @Override
    protected final void bindValues(SQLiteStatement stmt, Group entity) {
        stmt.clearBindings();
 
        Long id = entity.getId();
        if (id != null) {
            stmt.bindLong(1, id);
        }
        stmt.bindLong(2, entity.getPeerId());
        stmt.bindLong(3, entity.getGroupType());
        stmt.bindString(4, entity.getMainName());
        stmt.bindString(5, entity.getAvatar());
        stmt.bindLong(6, entity.getCreatorId());
        stmt.bindLong(7, entity.getUserCnt());
        stmt.bindString(8, entity.getUserList());
        stmt.bindLong(9, entity.getVersion());
        stmt.bindLong(10, entity.getStatus());
        stmt.bindLong(11, entity.getCreated());
        stmt.bindLong(12, entity.getUpdated());
    }

    @Override
    public Long readKey(Cursor cursor, int offset) {
        return cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0);
    }    

    @Override
    public Group readEntity(Cursor cursor, int offset) {
        Group entity = new Group( //
            cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0), // id
            cursor.getInt(offset + 1), // peerId
            cursor.getInt(offset + 2), // groupType
            cursor.getString(offset + 3), // mainName
            cursor.getString(offset + 4), // avatar
            cursor.getInt(offset + 5), // creatorId
            cursor.getInt(offset + 6), // userCnt
            cursor.getString(offset + 7), // userList
            cursor.getInt(offset + 8), // version
            cursor.getInt(offset + 9), // status
            cursor.getInt(offset + 10), // created
            cursor.getInt(offset + 11) // updated
        );
        return entity;
    }
     
    @Override
    public void readEntity(Cursor cursor, Group entity, int offset) {
        entity.setId(cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0));
        entity.setPeerId(cursor.getInt(offset + 1));
        entity.setGroupType(cursor.getInt(offset + 2));
        entity.setMainName(cursor.getString(offset + 3));
        entity.setAvatar(cursor.getString(offset + 4));
        entity.setCreatorId(cursor.getInt(offset + 5));
        entity.setUserCnt(cursor.getInt(offset + 6));
        entity.setUserList(cursor.getString(offset + 7));
        entity.setVersion(cursor.getInt(offset + 8));
        entity.setStatus(cursor.getInt(offset + 9));
        entity.setCreated(cursor.getInt(offset + 10));
        entity.setUpdated(cursor.getInt(offset + 11));
     }
    
    @Override
    protected final Long updateKeyAfterInsert(Group entity, long rowId) {
        entity.setId(rowId);
        return rowId;
    }
    
    @Override
    public Long getKey(Group entity) {
        if(entity != null) {
            return entity.getId();
        } else {
            return null;
        }
    }

    @Override
    public boolean hasKey(Group entity) {
        return entity.getId() != null;
    }

    @Override
    protected final boolean isEntityUpdateable() {
        return true;
    }
    
}
