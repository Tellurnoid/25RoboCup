function openInstagram() {
    // Instagramのアプリスキーム
    const appUrl = "instagram://user?username=tellurnoid";
    // Webブラウザで開くURL
    const webUrl = "https://www.instagram.com/tellurnoid";

    // モバイル環境のチェック
    const isMobile = /Mobi|Android/i.test(navigator.userAgent);

    if (isMobile) {
        // アプリスキームで開こうとする
        window.location = appUrl;

        // もしアプリがインストールされていない場合のためのフォールバック
        setTimeout(() => {
            // Webブラウザで開く
            window.location = webUrl;
        }, 2000); // 2秒待ってからWebにリダイレクト
    } else {
        // デスクトップ環境の場合は直接Webを開く
        window.location = webUrl;
    }
}

function openX_Daichi() {
    // xのアプリスキーム
    const appUrl = "x://user?screen_name=tellurnoid"; // xアプリでユーザープロフィールを開くURL
    // Webブラウザで開くURL
    const webUrl = "https://x.com/tellurnoid"; // Webブラウザで開くURL

    // モバイル環境のチェック
    const isMobile = /Mobi|Android/i.test(navigator.userAgent);

    if (isMobile) {
        // アプリスキームで開こうとする
        window.location = appUrl;

        // もしアプリがインストールされていない場合のためのフォールバック
        setTimeout(() => {
            // Webブラウザで開く
            window.location = webUrl;
        }, 2000); // 2秒待ってからWebにリダイレクト
    } else {
        // デスクトップ環境の場合は直接Webを開く
        window.location = webUrl;
    }
}
function openX_Tatsuki() {
    // xのアプリスキーム
    const appUrl = "x://user?screen_name=marudachi15"; // xアプリでユーザープロフィールを開くURL
    // Webブラウザで開くURL
    const webUrl = "https://x.com/marudachi15"; // Webブラウザで開くURL

    // モバイル環境のチェック
    const isMobile = /Mobi|Android/i.test(navigator.userAgent);

    if (isMobile) {
        // アプリスキームで開こうとする
        window.location = appUrl;

        // もしアプリがインストールされていない場合のためのフォールバック
        setTimeout(() => {
            // Webブラウザで開く
            window.location = webUrl;
        }, 2000); // 2秒待ってからWebにリダイレクト
    } else {
        // デスクトップ環境の場合は直接Webを開く
        window.location = webUrl;
    }
}
function openX_Sakuya() {
    // xのアプリスキーム
    const appUrl = "x://user?screen_name=UW_Russiania"; // xアプリでユーザープロフィールを開くURL
    // Webブラウザで開くURL
    const webUrl = "https://x.com/UW_Russiania"; // Webブラウザで開くURL

    // モバイル環境のチェック
    const isMobile = /Mobi|Android/i.test(navigator.userAgent);

    if (isMobile) {
        // アプリスキームで開こうとする
        window.location = appUrl;

        // もしアプリがインストールされていない場合のためのフォールバック
        setTimeout(() => {
            // Webブラウザで開く
            window.location = webUrl;
        }, 2000); // 2秒待ってからWebにリダイレクト
    } else {
        // デスクトップ環境の場合は直接Webを開く
        window.location = webUrl;
    }
}

function openYouTube() {
    // YouTubeのアプリスキーム
    const appUrl = "vnd.youtube://@カーボンノイド"; // YouTubeアプリでチャンネルを開くためのURL
    // Webブラウザで開くURL
    const webUrl = "https://www.youtube.com/@カーボンノイド/featured"; // Webブラウザで開くURL

    // モバイル環境のチェック
    const isMobile = /Mobi|Android/i.test(navigator.userAgent);

    if (isMobile) {
        // アプリスキームで開こうとする
        window.location = appUrl;

        // もしアプリがインストールされていない場合のためのフォールバック
        setTimeout(() => {
            // Webブラウザで開く
            window.location = webUrl;
        }, 2000); // 2秒待ってからWebにリダイレクト
    } else {
        // デスクトップ環境の場合は直接Webを開く
        window.location = webUrl;
    }
}
