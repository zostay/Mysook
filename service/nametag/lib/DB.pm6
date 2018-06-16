unit class DB;
use v6;

use DBIish;

has $!host     = %*ENV<DB_HOST>;
has $!user     = %*ENV<DB_USER>;
has $!password = %*ENV<DB_PASSWORD>;
has $!port     = %*ENV<DB_PORT>;
has $!database = %*ENV<DB_DATABASE>;

has $.dbh;

method connect() {
    $!dbh = DBIish.connect('mysql', :$!database, :$!host, :$!port, :$!user, :$!password, :RaiseError);
}

method txn(&code) {
    return code() if $*TXN;

    try { $!dbh.ping } or self.connect();

    my $result;
    my $retried = False;
    repeat {
        ENTER { $!dbh.do(q[START TRANSACTION]); }
        KEEP { $!dbh.do(q[COMMIT TRANSACTION]); }
        UNDO { $!dbh.do(q[ROLLBACK TRANSACTION]); }

        my $*TXN = True;

        $result = code($!dbh);

        CATCH {
            when (X::DBDish::DBerror) {
                .throw if $retried++;
                redo;
            }
        }

        1;
    } while False;

    return $result;
}

method create-schema() {
    try { $!dbh.ping } or self.connect();

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS program_v1(
            id NOT NULL AUTO_INCREMENT,
            name VARCHAR(100) NOT NULL,
            author VARCHAR(100) NOT NULL,
            descriptor BLOB,
            PRIMARY KEY (id)
        )
    ]);

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS queue_v1(
            id NOT NULL AUTO_INCREMENT,
            program_id INT NOT NULL,
            dequeued INT(1) NOT NULL DEFAULT 0,
            PRIMARY KEY (id)
        )
    ]);

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS nonce_v1(
            nonce INT NOT NULL,
            PRIMARY KEY (nonce)
        )
    ]);
}

method load-program(Int $id) {
    my %program;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            SELECT *
            FROM program_v1
            WHERE id = ?
        ]);
        $sth.execute($id);

        %program = $sth.row(:hash);
    }

    %program;
}

method save-program(Str $name, Str $author, Blob $descriptor) {
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            INSERT INTO program_v1(name, author, descriptor)
            VALUES (?, ?, ?)
        ]);
        $sth.execute($name, $author, $descriptor);

        self.enqueue($sth.insert-id);
    }
}

method list-programs() {
    my $sth = $!dbh.prepare(qq[
        SELECT id, name, author FROM program_v1 ORDER BY id DESC;
    ]);
    $sth.allrows(:array-of-hash);
}

method enqueue-program(Int $id) {
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            INSERT INTO queue_v1(program_id)
            VALUES (?)
        ]);
        $sth.execute($id);
    }
}

method dequeue-program() {
    my $descriptor;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            SELECT q.id, p.descriptor
            FROM queue_v1 q JOIN program_v1 p ON (q.program_id = p.id)
            ORDER BY q.dequeued DESC, q.id ASC, RAND()
            LIMIT 1;
        ]);
        $sth.execute;

        my $q-id;
        ($q-id, $descriptor) = $sth.row;

        if $q-id {
            $!dbh.do(qq[
                UPDATE queue_v1
                SET dequeued = 1
                WHERE id = ?
            ], $q-id);
        }
    }

    return $descriptor;
}

method check-nonce(Int $nonce) {
    my $okay = False;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            INSERT INTO nonce_v1(nonce)
            VALUES (?)
        ]);
        $sth.execute($nonce);

        # If that does not explode, this is a new nonce
        $okay = True;
    }

    return $okay;
}

method queue-length() {
    my $length = 0;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            SELECT COUNT(*)
            FROM queue_v1
            WHERE !dequeued
        ]);
        $sth.execute;
        ($length) = $sth.row;
    }

    return $length;
}

