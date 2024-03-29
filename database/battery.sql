
CREATE TABLE IF NOT EXISTS samples (
    sess INTEGER,
    minute INTEGER,
    unloaded NUMERIC(4,3),
    loaded NUMERIC(4,3),
    resistance NUMERIC(3,2),
    PRIMARY KEY(sess, minute)
);

CREATE TABLE IF NOT EXISTS sessions (
    id INTEGER PRIMARY KEY,
    created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    battery INTEGER
);
