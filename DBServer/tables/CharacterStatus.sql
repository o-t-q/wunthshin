CREATE TABLE characterstatus(
    user_id BIGSERIAL NOT NULL PRIMARY KEY,
    character_id BIGSERIAL NOT NULL,
    current_hp INT NOT NULL,
    current_exp INT NOT NULL
);
