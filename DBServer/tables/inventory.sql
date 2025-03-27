CREATE TABLE inventory(
    owner BIGSERIAL UNIQUE NOT NULL PRIMARY KEY,
    item_id BIGINT[] NOT NULL,
    item_count BIGINT[] NOT NULL
);
