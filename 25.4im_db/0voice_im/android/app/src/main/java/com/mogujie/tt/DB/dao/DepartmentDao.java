package com.mogujie.tt.DB.dao;

import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteStatement;

import de.greenrobot.dao.AbstractDao;
import de.greenrobot.dao.Property;
import de.greenrobot.dao.internal.DaoConfig;

import com.mogujie.tt.DB.entity.DepartmentEntity;

// THIS CODE IS GENERATED BY greenDAO, DO NOT EDIT.
/** 
 * DAO for table Department.
*/
public class DepartmentDao extends AbstractDao<DepartmentEntity, Long> {

    public static final String TABLENAME = "Department";

    /**
     * Properties of entity DepartmentEntity.<br/>
     * Can be used for QueryBuilder and for referencing column names.
    */
    public static class Properties {
        public final static Property Id = new Property(0, Long.class, "id", true, "_id");
        public final static Property DepartId = new Property(1, int.class, "departId", false, "DEPART_ID");
        public final static Property DepartName = new Property(2, String.class, "departName", false, "DEPART_NAME");
        public final static Property Priority = new Property(3, int.class, "priority", false, "PRIORITY");
        public final static Property Status = new Property(4, int.class, "status", false, "STATUS");
        public final static Property Created = new Property(5, int.class, "created", false, "CREATED");
        public final static Property Updated = new Property(6, int.class, "updated", false, "UPDATED");
    };


    public DepartmentDao(DaoConfig config) {
        super(config);
    }
    
    public DepartmentDao(DaoConfig config, DaoSession daoSession) {
        super(config, daoSession);
    }

    /** Creates the underlying database table. */
    public static void createTable(SQLiteDatabase db, boolean ifNotExists) {
        String constraint = ifNotExists? "IF NOT EXISTS ": "";
        db.execSQL("CREATE TABLE " + constraint + "'Department' (" + //
                "'_id' INTEGER PRIMARY KEY AUTOINCREMENT ," + // 0: id
                "'DEPART_ID' INTEGER NOT NULL UNIQUE ," + // 1: departId
                "'DEPART_NAME' TEXT NOT NULL UNIQUE ," + // 2: departName
                "'PRIORITY' INTEGER NOT NULL ," + // 3: priority
                "'STATUS' INTEGER NOT NULL ," + // 4: status
                "'CREATED' INTEGER NOT NULL ," + // 5: created
                "'UPDATED' INTEGER NOT NULL );"); // 6: updated
        // Add Indexes
        db.execSQL("CREATE INDEX " + constraint + "IDX_Department_DEPART_ID ON Department" +
                " (DEPART_ID);");
        db.execSQL("CREATE INDEX " + constraint + "IDX_Department_DEPART_NAME ON Department" +
                " (DEPART_NAME);");
    }

    /** Drops the underlying database table. */
    public static void dropTable(SQLiteDatabase db, boolean ifExists) {
        String sql = "DROP TABLE " + (ifExists ? "IF EXISTS " : "") + "'Department'";
        db.execSQL(sql);
    }

    /** @inheritdoc */
    @Override
    protected void bindValues(SQLiteStatement stmt, DepartmentEntity entity) {
        stmt.clearBindings();
 
        Long id = entity.getId();
        if (id != null) {
            stmt.bindLong(1, id);
        }
        stmt.bindLong(2, entity.getDepartId());
        stmt.bindString(3, entity.getDepartName());
        stmt.bindLong(4, entity.getPriority());
        stmt.bindLong(5, entity.getStatus());
        stmt.bindLong(6, entity.getCreated());
        stmt.bindLong(7, entity.getUpdated());
    }

    /** @inheritdoc */
    @Override
    public Long readKey(Cursor cursor, int offset) {
        return cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0);
    }    

    /** @inheritdoc */
    @Override
    public DepartmentEntity readEntity(Cursor cursor, int offset) {
        DepartmentEntity entity = new DepartmentEntity( //
            cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0), // id
            cursor.getInt(offset + 1), // departId
            cursor.getString(offset + 2), // departName
            cursor.getInt(offset + 3), // priority
            cursor.getInt(offset + 4), // status
            cursor.getInt(offset + 5), // created
            cursor.getInt(offset + 6) // updated
        );
        return entity;
    }
     
    /** @inheritdoc */
    @Override
    public void readEntity(Cursor cursor, DepartmentEntity entity, int offset) {
        entity.setId(cursor.isNull(offset + 0) ? null : cursor.getLong(offset + 0));
        entity.setDepartId(cursor.getInt(offset + 1));
        entity.setDepartName(cursor.getString(offset + 2));
        entity.setPriority(cursor.getInt(offset + 3));
        entity.setStatus(cursor.getInt(offset + 4));
        entity.setCreated(cursor.getInt(offset + 5));
        entity.setUpdated(cursor.getInt(offset + 6));
     }
    
    /** @inheritdoc */
    @Override
    protected Long updateKeyAfterInsert(DepartmentEntity entity, long rowId) {
        entity.setId(rowId);
        return rowId;
    }
    
    /** @inheritdoc */
    @Override
    public Long getKey(DepartmentEntity entity) {
        if(entity != null) {
            return entity.getId();
        } else {
            return null;
        }
    }

    /** @inheritdoc */
    @Override    
    protected boolean isEntityUpdateable() {
        return true;
    }
    
}
