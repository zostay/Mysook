use v6;

unit class DB;

use DBIish;

has $!host     = %*ENV<DB_HOST>;
has $!user     = %*ENV<DB_USER>;
has $!password = %*ENV<DB_PASSWORD>;
has $!port     = %*ENV<DB_PORT>;
has $!socket   = %*ENV<DB_SOCKET>;
has $!database = %*ENV<DB_DATABASE>;

has $.dbh;

method connect() {
    if $!socket {
        $!dbh = DBIish.connect('mysql', :$!database, :$!host, :$!socket, :$!user, :$!password, :RaiseError);
    }
    else {
        $!dbh = DBIish.connect('mysql', :$!database, :$!host, :$!port, :$!user, :$!password, :RaiseError);
    }
}

method txn(&code) {
    return code() if $*TXN;

    try { $!dbh.ping } or self.connect();

    my $result;
    my $retried = False;
    repeat {
        ENTER { $!dbh.do(q[BEGIN WORK]); }
        KEEP { $!dbh.do(q[COMMIT WORK]); }
        UNDO { $!dbh.do(q[ROLLBACK WORK]); }

        my $*TXN = True;

        $result = code($!dbh);

        CATCH {
            when (X::DBDish::DBError) {
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
            id INT NOT NULL AUTO_INCREMENT,
            name VARCHAR(100) NOT NULL,
            author VARCHAR(100) NOT NULL,
            descriptor BLOB,
            PRIMARY KEY (id)
        )
    ]);

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS queue_v2(
            id INT NOT NULL AUTO_INCREMENT,
            name VARCHAR(20) NOT NULL,
            PRIMARY KEY (id),
            UNIQUE (name)
        )
    ]);

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS queue_program_v1(
            id INT NOT NULL AUTO_INCREMENT,
            program_id INT NOT NULL,
            queue_id INT NOT NULL,
            dequeued INT(1) NOT NULL DEFAULT 0,
            PRIMARY KEY (id)
        )
    ]);

    $!dbh.do(qq[
        CREATE TABLE IF NOT EXISTS nonce_v2(
            nonce VARCHAR(10) NOT NULL,
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

        #self.enqueue-program($sth.insert-id);

        $sth.insert-id;
    }
}

method list-programs() {
    my $sth = $!dbh.prepare(qq[
        SELECT id, name, author FROM program_v1 ORDER BY id DESC;
    ]);
    $sth.execute;
    $sth.allrows(:array-of-hash);
}

method list-queue(Str $name) {
    my $sth = $!dbh.prepare(qq[
        SELECT p.id, p.name, p.author
          FROM program_v1 p
    INNER JOIN queue_program_v1 qp ON (p.id = qp.program_id)
    INNER JOIN queue_v2 q ON (q.id = qp.queue_id)
         WHERE q.name = ?
    ]);
    $sth.execute($name);
    $sth.allrows(:array-of-hash);
}

method enqueue-program(Str $name, Int $id --> Nil) {
    self.txn: {
        {
            my $sth = $!dbh.prepare(qq[
                INSERT IGNORE INTO queue_v2(name)
                VALUES (?)
            ]);
            $sth.execute($name);
        }

        my $queue-id = do {
            my $sth = $!dbh.prepare(qq[
                SELECT id FROM queue_v2 WHERE name = ?
            ]);
            $sth.execute($name);
            $sth.row.[0];
        }

        {
            my $sth = $!dbh.prepare(qq[
                INSERT INTO queue_program_v1(queue_id, program_id)
                VALUES (?, ?)
            ]);
            $sth.execute($queue-id, $id);

            $sth.insert-id;
        }

        Nil
    }
}

method dequeue-program($queue-name) {
    my $descriptor;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            SELECT qp.id, p.descriptor
              FROM queue_v2 q
              JOIN queue_program_v1 qp ON (qp.queue_id = q.id)
              JOIN program_v1 p ON (qp.program_id = p.id)
             WHERE q.name = ?
          ORDER BY q.dequeued DESC, q.id ASC, RAND()
              LIMIT 1;
        ]);
        $sth.execute($queue-name);

        my $qp-id;
        ($qp-id, $descriptor) = $sth.row;

        if $qp-id {
            $!dbh.do(qq[
                UPDATE queue_v2
                SET dequeued = 1
                WHERE id = ?
            ], $qp-id);
        }
    }

    return $descriptor;
}

subset Nonce of Str where /^ <[a..z A..Z 0..9]> ** 10 $/;

method check-nonce(Nonce $nonce) {
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

method queue-length($queue-name) {
    my $length = 0;
    self.txn: {
        my $sth = $!dbh.prepare(qq[
            SELECT COUNT(*)
              FROM queue_program_v1 qp
              JOIN queue_v2 q ON (qp.queue_id = q.id)
             WHERE !qp.dequeued AND q.name = ?
        ]);
        $sth.execute($queue-name);
        ($length) = $sth.row;
    }

    return $length;
}

