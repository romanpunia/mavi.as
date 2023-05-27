#include <std/schedule.as>
#include <std/http.as>
#include <std/console.as>

http::server@ server = null;

void exit_main()
{
    server.unlisten(1);
    schedule::get().stop();
}
int main()
{
    console@ output = console::get();
    schedule_policy policy; // Creates up to "CPU threads count" threads
    schedule@ queue = schedule::get();
    queue.start(policy);
    
    http::map_router@ router = http::map_router();
    router.listen("127.0.0.1", 8080);
    
    http::site_entry@ site = router.site("*");
    site.get("/", function(http::connection@ base)
    {
        base.response.set_header("content-type", "text/plain");
        base.response.content.assign("Hello, World!");
        base.finish(200);
    });
    site.get("/echo", function(http::connection@ base)
    {
        string content = co_await base.consume();
        base.response.set_header("content-type", "text/plain");
        base.response.content.assign(content);
        base.finish(200);
    });

    @server = http::server();
    server.configure(@router);
    server.listen();
    return 0;
}