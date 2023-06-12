#include "../../inc/main.h"

int main(int argc, char *argv[]) {

    // Ініціалізуємо бібліотеку OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Створюємо контекст SSL/TLS
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());

    // Встановлюємо зв'язок з сервером
    sock = connect_to_server();

    // Створюємо SSL об'єкт та пов'язуємо його з сокетом
    ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, sock);

    // Виконуємо SSL handshake
    SSL_connect(ssl);
    if (SSL_connect(ssl) != 1) {
        perror("SSL_connect");
        return -1;
    }

    GtkApplication *app = NULL;

    app = gtk_application_new("org.example.simple_gtk4_window", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(authorization), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    close(sock);
    g_object_unref(app);

    // Закриваємо з'єднання та очищуємо ресурси
    printf("Clean up\n");
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);
    close(sock);

    return status;
}
