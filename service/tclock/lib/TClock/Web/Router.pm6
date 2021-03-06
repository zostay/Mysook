use v6;

use Cofra::Web::Router::PathRouter;

unit class TClock::Web::Router is Cofra::Web::Router::PathRouter;

submethod ROUTE {
    get 'time' => %(
        defaults => %(
            controller => 'time',
            action     => 'fetch',
        ),
    );

    get 'time/:c' => %(
        defaults => %(
            controller => 'time',
            action     => 'fetch',
        ),
    );

    get 'time-zone' => %(
        defaults => %(
            controller => 'time-zone',
            action     => 'fetch',
        ),
    );

    put 'time-zone' => %(
        defaults => %(
            controller => 'time-zone',
            action     => 'store',
        ),
    );

    get 'alarm' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'fetch',
        ),
    );

    get 'alarm/:c1' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'fetch',
        ),
    );

    get 'alarm/:c1/:c2' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'fetch',
        ),
    );

    put 'alarm' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'store',
        ),
    );

    put 'alarm/:c1' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'store',
        ),
    );

    put 'alarm/:c1/:c2' => %(
        defaults => %(
            controller => 'alarm',
            action     => 'store',
        ),
    );
}
