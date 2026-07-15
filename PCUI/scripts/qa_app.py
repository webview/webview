from pathlib import Path
from playwright.sync_api import sync_playwright, expect


root = Path(__file__).resolve().parents[1]
output = root / "artifacts" / "qa"
output.mkdir(parents=True, exist_ok=True)

with sync_playwright() as playwright:
    browser = playwright.chromium.launch(headless=True)
    page = browser.new_page(viewport={"width": 1440, "height": 900}, device_scale_factor=1)
    errors = []
    page.on("console", lambda message: errors.append(message.text) if message.type == "error" else None)
    page.on("pageerror", lambda error: errors.append(str(error)))

    page.goto("http://127.0.0.1:5173/#/dashboard")
    page.wait_for_load_state("networkidle")
    expect(page.get_by_role("heading", name="下午好，林清越")).to_be_visible()
    page.screenshot(path=str(output / "dashboard-1440.png"), full_page=True)

    page.get_by_test_id("sidebar-toggle").click()
    expect(page.locator(".app-sidebar")).to_have_class("app-sidebar is-collapsed")

    page.get_by_role("button", name="切换语言").click()
    page.get_by_text("English", exact=True).click()
    expect(page.get_by_text("Dashboard", exact=True).first).to_be_visible()
    expect(page.locator(".page-title")).to_have_css("font-size", "28px")
    expect(page.locator(".page-title")).to_have_css("line-height", "36px")
    expect(page.locator(".page-description")).to_have_css("font-size", "14px")
    expect(page.locator(".sidebar-link").first).to_have_css("font-size", "14px")
    expect(page.locator(".metric-value").first).to_have_css("font-size", "24px")
    expect(page.locator(".p-button").first).to_have_css("font-size", "14px")

    page.get_by_role("button", name="Change theme").click()
    page.get_by_text("Dark", exact=True).click()
    expect(page.locator("html")).to_have_class("app-dark")
    global_search = page.get_by_role("searchbox", name="Search anything")
    global_search.fill("Projects")
    expect(page.get_by_test_id("global-search-results").get_by_text("Projects", exact=True)).to_be_visible()
    global_search.press("Escape")
    expect(page.get_by_role("button", name="Minimize")).to_be_visible()
    expect(page.get_by_role("button", name="Maximize")).to_be_visible()
    expect(page.get_by_role("button", name="Close window")).to_be_visible()
    expect(page.get_by_label("Account menu")).to_be_visible()

    for width in (1280, 1024):
        page.set_viewport_size({"width": width, "height": 800})
        page.wait_for_timeout(120)
        overflow = page.evaluate("document.documentElement.scrollWidth > document.documentElement.clientWidth")
        assert not overflow, f"Horizontal overflow at {width}px"
        if width == 1024:
            expect(page.get_by_test_id("sidebar-toggle")).to_have_count(0)
        page.screenshot(path=str(output / f"dashboard-{width}.png"), full_page=True)

    for route in ("projects", "tasks", "files", "analytics", "team", "messages", "calendar", "assistant", "profile", "settings"):
        page.goto(f"http://127.0.0.1:5173/#/{route}")
        page.wait_for_load_state("networkidle")
        assert page.locator("main").count() == 1
        unnamed_buttons = page.locator("button").evaluate_all("buttons => buttons.filter(button => !(button.getAttribute('aria-label') || button.getAttribute('title') || button.textContent.trim())).map(button => button.outerHTML)")
        assert not unnamed_buttons, f"Unnamed buttons on {route}: {unnamed_buttons}"
        if route == "assistant":
            expect(page.locator(".p-drawer")).to_be_visible()

    page.goto("http://127.0.0.1:5173/#/calendar")
    expect(page.get_by_text("July 2026", exact=True)).to_be_visible()
    page.get_by_role("button", name="Next month").click()
    expect(page.get_by_text("August 2026", exact=True)).to_be_visible()
    expect(page.get_by_text("Release check", exact=True).first).to_be_visible()
    expect(page.get_by_role("button", name="Thursday, August 6, 2026")).to_be_visible()

    page.goto("http://127.0.0.1:5173/#/messages")
    page.get_by_placeholder("Search messages").fill("Zhou")
    expect(page.get_by_text("Zhou Yu", exact=True).first).to_be_visible()
    expect(page.get_by_text("Shen Xing", exact=True)).to_have_count(0)

    page.goto("http://127.0.0.1:5173/#/team")
    page.get_by_placeholder("Search members").fill("Gu Yan")
    expect(page.get_by_text("Gu Yan", exact=True)).to_be_visible()
    expect(page.get_by_text("Lin Qingyue", exact=True)).to_have_count(0)

    page.goto("http://127.0.0.1:5173/#/files")
    expect(page.get_by_text("Product-guidelines.pdf", exact=True)).to_be_visible()
    expect(page.locator(".panel-heading h2").first).to_have_css("font-size", "20px")
    expect(page.locator(".p-datatable-thead th").first).to_have_css("font-size", "14px")
    expect(page.locator(".p-datatable-tbody td").first).to_have_css("font-size", "14px")
    upload_input = page.locator('input[type="file"]')
    upload_input.set_input_files({"name": "qa-notes.txt", "mimeType": "text/plain", "buffer": b"qa"})
    expect(page.get_by_text("qa-notes.txt", exact=True)).to_be_visible()
    page.get_by_role("row").filter(has_text="qa-notes.txt").get_by_role("button", name="Delete file").click()
    expect(page.get_by_text("qa-notes.txt", exact=True)).to_have_count(0)

    assert not errors, "Browser console errors: " + " | ".join(errors)
    browser.close()
