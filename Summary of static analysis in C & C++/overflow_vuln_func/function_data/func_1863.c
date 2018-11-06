static void kvm_hwpoison_page_add(ram_addr_t ram_addr)
{
    HWPoisonPage *page;

    QLIST_FOREACH(page, &hwpoison_page_list, list) {
        if (page->ram_addr == ram_addr) {
            return;
        }
    }
    page = g_malloc(sizeof(HWPoisonPage));
    page->ram_addr = ram_addr;
    QLIST_INSERT_HEAD(&hwpoison_page_list, page, list);
}
