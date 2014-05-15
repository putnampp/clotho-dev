#ifndef EVENT_PAGE_WALKER_HPP_
#define EVENT_PAGE_WALKER_HPP_

// Forward Iterator for walking events in page relative object order
//
template < class EP >
class EventPageWalker {
public:
    typedef EP event_page_t;

    typedef typename event_page_t::event_node event_node_t;
    typedef typename event_page_t::object_node object_node_t;

    typedef typename event_page_t::event_t event_t;
    typedef typename event_page_t::object_t object_t;

    typedef event_t & event_reference;
    typedef event_t * event_pointer;

    typedef object_t object_reference;

    EventPageWalker( event_page_t * page = NULL ) : 
        m_page(page),
        m_obj( ((page != NULL) ? page->head_object() : NULL ) ),
        m_stop_obj( ((page != NULL) ? page->end_object() : NULL ) ),
        m_en( ((m_obj != NULL )? m_obj->enode : NULL )),
        m_bObjectChange(m_obj != m_stop_obj)
    { }

    EventPageWalker( const EventPageWalker< EP > & epw ) : 
        m_page( epw.m_page ),
        m_obj( epw.m_obj ),
        m_stop_obj( epw.m_stop_obj ),
        m_en( epw.m_en ),
        m_bObjectChange( epw.m_bObjectChange )
    {}

    EventPageWalker< EP > & operator++() {
        if( m_page != NULL ) {
            if( m_en->next == NULL ) {  // transition to next object with an event
                do {
                    if( ++m_obj > m_stop_obj ) {
                        m_en = NULL;
                        m_page = NULL;
                        m_obj = NULL;
                        break;
                    }
                    m_en = m_obj->enode;
                } while( m_en == NULL );
                m_bObjectChange = true;
            } else {
                m_en = m_en->next;
                m_bObjectChange = false;
            }
        }

        return *(this);
    }

    bool operator!=( const EventPageWalker< EP > & rhs ) const {
        return !(operator==(rhs));
    }

    bool operator==( const EventPageWalker< EP > & rhs ) const {
        return m_page == rhs.m_page
                && m_obj == rhs.m_obj
                && m_en == rhs.m_en;
    }

    bool HasObjectChanged() const { return m_bObjectChange; }

    EventPageWalker< EP > & operator=( const EventPageWalker< EP > & rhs ) {
        if( this != &rhs ) {
            m_page = rhs.m_page;
            m_obj = rhs.m_obj;
            m_stop_obj = rhs.m_stop_obj;
            m_en = rhs.m_en;
        }
        return *(this);
    }

    event_reference getEvent() { return *(m_en->p_event); }

    object_reference getObjectID() { return m_obj->object_id; }

    virtual ~EventPageWalker() {    }
protected:
    event_page_t    * m_page;
    object_node_t   * m_obj, *m_stop_obj;
    event_node_t    * m_en;

    bool    m_bObjectChange;
};

#endif  // EVENT_PAGE_WALKER_HPP_
