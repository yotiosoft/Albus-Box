function switch_to_windows() {
    let current_option = document.getElementsByClassName("os_select is_active")[0];
    current_option.classList.remove("is_active");

    let selected_option = document.getElementById("select_windows");
    selected_option.classList.add("is_active");

    let current_view = document.getElementsByClassName("os_document is_active")[0];
    current_view.classList.remove("is_active");

    let selected_view = document.getElementById("switch_view_windows");
    selected_view.classList.add("is_active");
}

function switch_to_macos() {
    let current_option = document.getElementsByClassName("os_select is_active")[0];
    current_option.classList.remove("is_active");

    let selected_option = document.getElementById("select_macos");
    selected_option.classList.add("is_active");

    let current_view = document.getElementsByClassName("os_document is_active")[0];
    current_view.classList.remove("is_active");

    let selected_view = document.getElementById("switch_view_macos");
    selected_view.classList.add("is_active");
}

function switch_to_linux() {
    let current_option = document.getElementsByClassName("os_select is_active")[0];
    current_option.classList.remove("is_active");

    let selected_option = document.getElementById("select_linux");
    selected_option.classList.add("is_active");

    let current_view = document.getElementsByClassName("os_document is_active")[0];
    current_view.classList.remove("is_active");

    let selected_view = document.getElementById("switch_view_linux");
    selected_view.classList.add("is_active");
}
